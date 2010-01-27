using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Text.RegularExpressions;
using System.Xml;
using Microsoft.VisualStudio.VCProjectEngine;

namespace sln2mak
{
    class VcSlnInfo
    {
        #region Members
        /****************************************************************/
        private string                              m_MainProjectName       ;
        private string                              m_SlnFullPath           ;
        private Dictionary<string, string>          m_ProjGuidName          ;
        private Dictionary<string, string>          m_ProjNamePath          ;
        private Dictionary<string, string>          m_ProjMakFilePath       ;
        private Dictionary<string, string[]>        m_ProjDependencies      ;
        Dictionary<string, string>.KeyCollection    m_ProjPathCollection    ;
        private string                              m_MakefileFullPath      ;
        private StreamWriter                        m_MakefileWriter        ;
        /****************************************************************/
        #endregion /* Members */

        #region LifeCycle
        /****************************************************************/
        /// <summary>
        /// Use this constructor when parsing .sln file
        /// </summary>
        /// <param name="MainProjName"></param>
        /// <param name="SlnFileName"></param>
        public VcSlnInfo (string MainProjName , string SlnFileName)
        {
            m_MainProjectName   = MainProjName  ;
            m_SlnFullPath       = SlnFileName   ;
            if (!(Path.GetDirectoryName(m_SlnFullPath).Equals("")))
            {
                //m_MakefileFullPath = Path.GetDirectoryName(m_SlnFullPath) + "/Makefile";
                m_MakefileFullPath = Path.GetDirectoryName(m_SlnFullPath) + "/CMakeLists.txt";
            }
            else
            {
                m_MakefileFullPath = "CMakeLists.txt";
            }

            Console.WriteLine("Creating file " + m_MakefileFullPath + "...");
            m_MakefileWriter    = new StreamWriter(m_MakefileFullPath) ;

            m_ProjGuidName      = new Dictionary<string, string>   () ;
            m_ProjNamePath      = new Dictionary<string, string>   () ;
            m_ProjMakFilePath   = new Dictionary<string, string>   () ;
            m_ProjDependencies  = new Dictionary<string, string[]> () ;

            m_MakefileWriter.WriteLine("# CMakeLists.txt - Created with Sln2CMake Application");
            m_MakefileWriter.WriteLine();

            iInitDictionariesFromSlnFile () ;
        }

        /// <summary>
        /// Use this constructor when parsing .vcproj list of file and then creating Makefile
        /// </summary>
        /// <param name="MainProjName"></param>

        public VcSlnInfo (string MainProjNameFullPath)
        {
            m_MainProjectName   = Path.GetFileNameWithoutExtension(MainProjNameFullPath)    ;
            m_SlnFullPath       = ""                                                        ;
            if (Path.GetDirectoryName(MainProjNameFullPath).Equals(""))
            {
                //m_MakefileFullPath = Path.GetDirectoryName(MainProjNameFullPath) + "/Makefile";
                m_MakefileFullPath = Path.GetDirectoryName(MainProjNameFullPath) + "CMakeLists.txt";
            }
            else
            {
                m_MakefileFullPath = "CMakeLists.txt";
            }

            m_MakefileWriter    = new StreamWriter(m_MakefileFullPath) ;

            m_ProjGuidName      = new Dictionary<string , string>   () ;
            m_ProjNamePath      = new Dictionary<string , string>   () ;
            m_ProjMakFilePath   = new Dictionary<string , string>   () ;
            m_ProjDependencies  = new Dictionary<string , string[]> () ;

            m_MakefileWriter.WriteLine("# CMakeLists.txt - Created with Sln2CMake Application");
            m_MakefileWriter.WriteLine (                                     ) ;            
        }
        /****************************************************************/
        #endregion /* LifeCycle */

        #region Private Operations
        /****************************************************************/
        /// <summary>
        /// Parse sln and initialize dictionaries with projects names, pathes and makefile names
        /// </summary>        
        private void iInitDictionariesFromSlnFile ()
        {
            StreamReader    sr              ;
            Match           matchProjInfo   ;
            String          vcprojPath      ;
            String          makeFileName    ;
            string          line            ;


            sr = new StreamReader (m_SlnFullPath) ;


            //Find all .vcpoj files in .sln and add their info to dictionaries
            while ((line = sr.ReadLine()) != null)
            {
                matchProjInfo = Parser.ProjectRegex.Match(line);
                if (matchProjInfo.Success)
                {
                    //Key = Guid number, Value = ProjectName
                    m_ProjGuidName.Add(matchProjInfo.Groups[4].Value, matchProjInfo.Groups[2].Value);

                    //replace slashes for matching Linux OS
                    vcprojPath = matchProjInfo.Groups[3].Value.Replace("\\", "/");
                    //Key = ProjectName , Value = ProjectFullPath
                    m_ProjNamePath.Add(matchProjInfo.Groups[2].Value, vcprojPath);

                    //replace .vcproj extention to .mak for makefile name
                    makeFileName = vcprojPath;

                    makeFileName = makeFileName.Replace(".vcproj", ".mak");
                    // If we run with full path to .sln file...
                    if (!(Path.GetDirectoryName(m_SlnFullPath).Equals(""))) 
                    {
                        // Add full path to .mak file to dictionary
                        // to make possible to create it.
                        // Beginning of the path will be deleted before write to Makefile.
                        makeFileName = Path.GetDirectoryName(m_SlnFullPath) + "\\" + makeFileName;
                        makeFileName = makeFileName.Replace("\\", "/");
                    }

                    //Key = ProjectName , Value = MakeFileName
                    m_ProjMakFilePath.Add(matchProjInfo.Groups[2].Value, makeFileName);

                }

                if (line.StartsWith("Global"))
                {
                    sr.Close();
                    break;
                };
            }

            m_ProjPathCollection = m_ProjNamePath.Keys;

            foreach (string projectName in m_ProjPathCollection)
            {
                string[] dependencies = iInitProjectDependenciesFromSlnFile (projectName) ;
                m_ProjDependencies.Add(projectName, dependencies);
            }
        }

        /// <summary>
        /// Initialize solution(sln) dependencies
        /// </summary>
        private string[] iInitProjectDependenciesFromSlnFile( String projectName)
        {
            StreamReader sr;
            Regex regGuid;
            Match matchProjInfo;
            string line;
            String dependencies = "";


            //read file again from the start
            sr = new StreamReader(m_SlnFullPath);

            regGuid = new Regex(@"(.*)\{(.*)\} = \{(.*)\}");

            //create dependencies for the main project - projName            
            while ((line = sr.ReadLine()) != null)
            {
                matchProjInfo = Parser.ProjectRegex.Match(line);

                if (matchProjInfo.Success)
                {
                    if (matchProjInfo.Groups[2].Value.Equals(projectName))     //main project
                    {
                        //read main project dependencies within 'ProjectDependencies' section
                        if ((line = sr.ReadLine()).Equals("\tProjectSection(ProjectDependencies) = postProject"))
                        {
                            while (!(line = sr.ReadLine()).Equals("\tEndProjectSection"))
                            {
                                Match matchGuid = regGuid.Match(line);
                                //add main project dependencies to projDependencies string
                                if (matchGuid.Success)
                                {
                                    dependencies += m_ProjGuidName[matchGuid.Groups[2].Value] + " ";
                                }
                            }
                        }
                    }
                }
            }
            sr.Close();

            //return string array with split solution(sln) dependencies (no empty entries, delimiter is ' ')            
            return (dependencies.Split(new Char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries));
        }

        /// <summary>
        /// Print the default target rule --PHONY:all
        /// first for secondary(depended projects) then for main one
        /// </summary>
        /// <param name="projName">main project name</param>
        /// <param name="sw">StreamWriter pointer to Makefile</param>
        /// <param name="keyColl">Collection of project names</param>
        /// <param name="slnDependencies">main project dependencies</param>
        private void iPrintDefaultTargetRule ()
        {
            m_MakefileWriter.WriteLine(".PHONY: all");
            m_MakefileWriter.WriteLine("all: \\");

            //Print the default target rule
            //First add depended projects 
            foreach (string projectName in m_ProjPathCollection)
            {
                for (int d = 0; d < m_ProjDependencies[m_MainProjectName].Length; d++)
                {
                    if ((!(m_MainProjectName.Equals(projectName)                    )) &&
                        (projectName.Equals(m_ProjDependencies[m_MainProjectName][d]))   )
                    {
                        m_MakefileWriter.WriteLine(" \t{0} \\", projectName);
                    }
                }
            }

            //Then add main project_name to makefile
            foreach (string projectName in m_ProjPathCollection)
            {
                if (m_MainProjectName.Equals(projectName))
                {
                    m_MakefileWriter.WriteLine(" \t{0}", projectName);
                }
            }
            m_MakefileWriter.WriteLine();
            m_MakefileWriter.WriteLine();
        }
        
        /// <summary>
        /// Print the rules for each project target
        /// </summary>      
        private void iPrintTargetRules()
        {
            int j = 0;

            //Print the rules for main project first
            foreach (string projectName in m_ProjPathCollection)
            {
                if (m_MainProjectName.Equals(projectName))
                {
                    m_MakefileWriter.WriteLine(".PHONY: {0}", projectName);

                    int k = 0;
                    String pd_tmp = "";

                    for (int d = 0; d < m_ProjDependencies[m_MainProjectName].Length; d++)
                    {

                        pd_tmp = pd_tmp + " " + m_ProjDependencies[m_MainProjectName][d];
                        k++;                        
                    }

                    m_MakefileWriter.WriteLine("{0}:{1}", projectName, pd_tmp);

                    if (!(m_ProjMakFilePath[projectName].Equals("")))
                    {
                        Int32 stripStart = Path.GetDirectoryName(m_SlnFullPath).Length + 1;
                        Int32 stripLength = m_ProjMakFilePath[projectName].Length - stripStart - 4 - projectName.Length;
                        String projMakFilePath = m_ProjMakFilePath[projectName].Substring(0, stripStart);//, stripLength);
                        m_MakefileWriter.WriteLine("\tcd {0} && $(MAKE) -f {1}", projMakFilePath, projectName + ".mak");
                    }
                    m_MakefileWriter.WriteLine();
                }

            }
            //Print the rules for depended projects 
            foreach (string projectName in m_ProjPathCollection)
            {
                String pd_tmp = "";

                if (!(m_MainProjectName.Equals(projectName)))
                {
                    for (int k = 0; k < m_ProjDependencies[projectName].Length ; k++)
                    {
                        pd_tmp = pd_tmp + " " + m_ProjDependencies[projectName][k];
                    }                        

                    m_MakefileWriter.WriteLine(".PHONY: {0}", projectName);
                    m_MakefileWriter.WriteLine("{0}:{1}", projectName, pd_tmp /*m_ProjDependencies[projectName]*/);

                    if (!(m_ProjMakFilePath[projectName].Equals("")))
                    {
                        Int32 stripStart = Path.GetDirectoryName(m_SlnFullPath).Length + 1;
                        Int32 stripLength = m_ProjMakFilePath[projectName].Length - stripStart - 4 - projectName.Length;
                        String projMakFilePath = m_ProjMakFilePath[projectName].Substring(stripStart, stripLength);
                        m_MakefileWriter.WriteLine("\tcd {0} && $(MAKE) -f {1}", projMakFilePath, projectName+".mak");
                    }
                    m_MakefileWriter.WriteLine();
                }
                j++;
            }
        }

        /// <summary>
        /// Print the 'clean' or 'depends' target rule first for main project, then for all depended projects
        /// </summary>
        /// <param name="projName">main project name</param>
        /// <param name="sw">StreamWriter pointer to Makefile</param>
        /// <param name="keyColl">Collection of project names</param>
        /// <param name="mainProjDependencies">main project dependencies</param>
        private void iPrintClenOrDependsRules(string ruleString)
        {
            m_MakefileWriter.WriteLine(".PHONY: {0}", ruleString);
            m_MakefileWriter.WriteLine("{0}:", ruleString);

            //Print the 'depends' target rule for main project first
            foreach (string projectName in m_ProjPathCollection)
            {

                if (projectName.Equals(m_MainProjectName))
                {
                    if (!(m_ProjMakFilePath[m_MainProjectName].Equals("")))
                    {
                        Int32 stripStart = Path.GetDirectoryName(m_SlnFullPath).Length + 1;
                        Int32 stripLength = m_ProjMakFilePath[projectName].Length - stripStart - 4 - projectName.Length;
                        String projMakFilePath = m_ProjMakFilePath[m_MainProjectName].Substring(stripStart, stripLength);
                        m_MakefileWriter.WriteLine("\tcd {0} && $(MAKE) -f {1} {2}", projMakFilePath, m_MainProjectName + ".mak", ruleString);
                    }
                }
            }

            //Print the 'depends' target rule for depended projects 
            foreach (string projectName in m_ProjPathCollection)
            {
                if (!(m_MainProjectName.Equals(projectName)))
                {
                    for (int d = 0; d < m_ProjDependencies[m_MainProjectName].Length; d++)
                    {
                        if (!(m_ProjMakFilePath[projectName].Equals("")))
                        {
                            Int32 stripStart = Path.GetDirectoryName(m_SlnFullPath).Length + 1;
                            Int32 stripLength = m_ProjMakFilePath[projectName].Length - stripStart - 4 - projectName.Length;
                            String projMakFilePath = m_ProjMakFilePath[projectName].Substring(stripStart, stripLength);
                            m_MakefileWriter.WriteLine("\tcd {0} && $(MAKE) -f {1} {2}", projMakFilePath, projectName + ".mak", ruleString);
                        }
                    }
                }
            }
            m_MakefileWriter.WriteLine();
        }

        /// <summary>
        /// Call to ParseVsproj for all vcproj files in sln: main and main's depended projects
        /// </summary>
        private void iSendToParseVcprojFiles()
        {
            VcProjInfo vcProjInfo;



            // parse main project file.vcproj
            foreach (string projectName in m_ProjPathCollection)
            {
                if (projectName.Equals(m_MainProjectName))
                {
                    if (!(m_ProjMakFilePath[projectName].Equals("")))
                    {
                        if (Path.GetDirectoryName(m_SlnFullPath).Equals(""))
                        {
                            vcProjInfo = new VcProjInfo(m_ProjNamePath[projectName], m_ProjMakFilePath[projectName], m_ProjDependencies[projectName]);
                            vcProjInfo.ParseVcproj();
                        }
                        else
                        {
                            vcProjInfo = new VcProjInfo(Path.GetDirectoryName(m_SlnFullPath).Replace("\\", "/") + "\\" + m_ProjNamePath[projectName], m_ProjMakFilePath[projectName], m_ProjDependencies[projectName]);
                            vcProjInfo.ParseVcproj();
                        }
                    }
                }
            }

            // parse depended project files
            foreach (string projectName in m_ProjPathCollection)
            {

                for (int d = 0; d < m_ProjDependencies[m_MainProjectName].Length; d++)
                {
                    if ((!(m_MainProjectName.Equals(projectName)                    )) &&
                        (projectName.Equals(m_ProjDependencies[m_MainProjectName][d]))    )
                    {
                        if (!(m_ProjMakFilePath[projectName].Equals("")))
                        {                            
                            if (Path.GetDirectoryName(m_SlnFullPath).Equals(""))
                            {
                                vcProjInfo = new VcProjInfo(m_ProjNamePath[projectName], m_ProjMakFilePath[projectName], m_ProjDependencies[projectName]/*dependencies*/);
                                vcProjInfo.ParseVcproj();
                            }
                            else
                            {
                                vcProjInfo = new VcProjInfo(Path.GetDirectoryName(m_SlnFullPath).Replace("\\", "/") + "/" + m_ProjNamePath[projectName], m_ProjMakFilePath[projectName], m_ProjDependencies[projectName]/*dependencies*/);
                                vcProjInfo.ParseVcproj();
                            }

                        }
                    }
                }
            }
        }
        
        /****************************************************************/
        #endregion /* Private Operations */

        #region Public Operations
        /****************************************************************/
        /// <summary>
        /// Generates Makefile, call GenerateMakefile with true value
        /// It useful in case there is need in vcproj parsing. 
        /// Actually when application called with .sln argument
        /// this function will be called by parser
        /// </summary>
        public void GenerateMakefile ()
        {
            //GenerateMakefile(true);
            GenerateCMakeFile();
        }

        public bool IsFileExcludedFromBuild(XmlNode FileNode, string strConfigName)
        {
            XmlNode ExcludeNode = FileNode.SelectSingleNode("./FileConfiguration[@Name=\'" + strConfigName + "\' and @ExcludedFromBuild=\'true\']");
            return ExcludeNode != null;
        }
        
        public bool GenerateCMakeForProject(string strProjectName, XmlDocument doc)
        {
            string strConfigName = "Debug|Win32";//VisualStudioProject/Configurations/Configuration


            XmlNode ConfigNode = doc.SelectSingleNode("/VisualStudioProject/Configurations/Configuration[@Name=\'" + strConfigName + "\']");
            if (ConfigNode == null)
            {
                Console.WriteLine("    Project {0} is unsupported type. Only Win32 type projects supported as of now", strProjectName);
                return false;
            }
            
            ConfigurationTypes nConfigType = (ConfigurationTypes)Convert.ToInt32(ConfigNode.Attributes["ConfigurationType"].Value);

            if (nConfigType == ConfigurationTypes.typeGeneric || nConfigType == ConfigurationTypes.typeUnknown)
            {
                Console.WriteLine("    Project {0} is unsupported type. Only Application and Library types are supported at present", strProjectName);
                return false; // we do not support these type of projects
            }

            // Find the Target type exe or lib ??
            string strProjectType = (nConfigType == ConfigurationTypes.typeApplication ? "executable" : "library");
            string strShared = (nConfigType == ConfigurationTypes.typeDynamicLibrary ? " SHARED " : "");
            string strProjectFileGroups = "";

            // Find the Target name
            string strTargetName = strProjectName; //todo: Find the real target name given the project name

            // Compute the location of the Project file
            string strBasePath = Path.GetDirectoryName(this.m_ProjNamePath[strProjectName]).Replace('\\','/');
            if (strBasePath.Length > 0) strBasePath += '/';
            
            // Print the Include and Lib directory information
            XmlNode CLNode = doc.SelectSingleNode("/VisualStudioProject/Configurations/Configuration[@Name=\'" + strConfigName + "\']/Tool[@Name=\'VCCLCompilerTool\']");
            if (CLNode != null)
            {
                try
                {
                    string strIncludeDirs = CLNode.Attributes["AdditionalIncludeDirectories"].Value;
                    if (strIncludeDirs.Length > 0)
                    {
                        if(strIncludeDirs.EndsWith(";")) strIncludeDirs = strIncludeDirs.Remove(strIncludeDirs.Length-1);
                        strIncludeDirs = strBasePath+strIncludeDirs.Replace(";", ";"+strBasePath);
                        strIncludeDirs = "\"" + strIncludeDirs.Replace(";", "\" \"") + "\"";
                        m_MakefileWriter.WriteLine("include_directories( {0} )", strIncludeDirs.Replace('\\', '/'));
                    }
                }
                catch(Exception )
                {
                    Console.WriteLine("{0} has No AdditionalIncludeDirectories specified", strProjectName);
                }
            }
            XmlNode LinkerNode = doc.SelectSingleNode("/VisualStudioProject/Configurations/Configuration[@Name=\'" + strConfigName + "\']/Tool[@Name=\'VCLinkerTool\']");
            if (LinkerNode != null)
            {
                try
                {
                    string strLibDirs = LinkerNode.Attributes["AdditionalLibraryDirectories"].Value;
                    if (strLibDirs.Length > 0)
                    {
                        if (strLibDirs.EndsWith(";")) strLibDirs = strLibDirs.Remove(strLibDirs.Length - 1);
                        strLibDirs = strBasePath + strLibDirs.Replace(";", ";" + strBasePath);
                        strLibDirs = "\"" + strLibDirs.Replace(";", "\" \"") + "\"";
                        m_MakefileWriter.WriteLine("link_directories( {0} )", strLibDirs.Replace('\\', '/'));
                    }
                }
                catch (Exception )
                {
                    Console.WriteLine("    {0} has No AdditionalLibraryDirectories specified", strProjectName);
                }
            }

            // Print the Additional Libraries
            XmlNode LibrarianNode = doc.SelectSingleNode("/VisualStudioProject/Configurations/Configuration[@Name=\'" + strConfigName + "\']/Tool[@Name=\'VCLibrarianTool\']");
            if (LibrarianNode != null)
            {
                try
                {
                    string strAdditionalDependencies = LibrarianNode.Attributes["AdditionalDependencies"].Value;
                    //if (strAdditionalDependencies.EndsWith(";")) strAdditionalDependencies = strAdditionalDependencies.Remove(strAdditionalDependencies.Length - 1);
                    //if (strAdditionalDependencies.EndsWith(",")) strAdditionalDependencies = strAdditionalDependencies.Remove(strAdditionalDependencies.Length - 1);
                    //strAdditionalDependencies = strBasePath + strAdditionalDependencies.Replace(";", ";" + strBasePath);
                    //strAdditionalDependencies = strBasePath + strAdditionalDependencies.Replace(",", ";" + strBasePath);
                    strAdditionalDependencies = " " + strAdditionalDependencies.Replace(";", " ") + " ";
                    strAdditionalDependencies = " " + strAdditionalDependencies.Replace(",", " ") + " ";
                    strAdditionalDependencies.Replace('\\', '/');
                    m_MakefileWriter.WriteLine("SET( {0} )", strProjectName + "_Librarian " + strAdditionalDependencies.Replace('\\', '/'));
                }
                catch (Exception)
                {
                    Console.WriteLine("    {0} has No AdditionalDependencies", strProjectName);
                }
            }

            // Move on to add the individual files
            XmlNodeList FilesNodeList = doc.SelectNodes("/VisualStudioProject/Files");
            
            if(FilesNodeList.Count > 0)
            {
                string strFilePaths = ""; // to hold any un-filtered files

                //Files has Filters
                foreach (XmlNode FileOrFilter in FilesNodeList[0].ChildNodes)
                {
                    if(FileOrFilter.Name.ToUpper() == "FILTER") // this is a filter node
                    {
                        string strFileGroup = FileOrFilter.Attributes["Name"].Value;

                        strFileGroup = strFileGroup.Replace(' ', '_');

                        if(FileOrFilter.ChildNodes.Count > 0)
                        {
                            m_MakefileWriter.WriteLine("SET( {0} ", strProjectName + "_" + strFileGroup);
                            foreach(XmlNode File in FileOrFilter.ChildNodes)
                            {
                                if (IsFileExcludedFromBuild(File, strConfigName) == false)
                                {
                                    string strFilePath = File.Attributes["RelativePath"].Value;
                                    m_MakefileWriter.WriteLine("\t{0}", strBasePath + strFilePath.Replace('\\', '/'));
                                }
                                else
                                    Console.WriteLine("\t{0} not selected for Build", File.Attributes["RelativePath"].Value);
                            }
                            m_MakefileWriter.WriteLine("   )", strProjectName + "_" + strFileGroup);

                            strProjectFileGroups += (" ${" + strProjectName + "_" + strFileGroup + "} ");
                        }
                    }
                    else if (FileOrFilter.Name.ToUpper() == "FILE")
                    {
                        if (IsFileExcludedFromBuild(FileOrFilter, strConfigName) == false)
                        {
                            string strFilePath = FileOrFilter.Attributes["RelativePath"].Value;
                            strFilePaths += (strFilePath.Replace('\\', '/') + "   ");
                        }
                        else
                            Console.WriteLine("    {0} not selected for Build", FileOrFilter.Attributes["RelativePath"].Value);
                    }
                }

                if(strFilePaths.Length > 0) // if there were any un-filtered files
                {
                    m_MakefileWriter.WriteLine("SET( {0} )", strProjectName + "_UnKnownGroup " + strFilePaths);
                    strProjectFileGroups += (" ${" + strProjectName + "_UnKnownGroup" + "} ");
                }
            }
            
            // Generate the Target build rule
            if (strProjectFileGroups.Length > 0)
            {
                m_MakefileWriter.WriteLine();
                m_MakefileWriter.WriteLine("add_{0}({1} {2} {3})", strProjectType, strTargetName, strShared, strProjectFileGroups);
            }

            m_MakefileWriter.WriteLine();

            return true;
        }

        public void GenerateCMakeFile()
        {
            Dictionary<string, bool> bProjectBuildRuleSuccess = new Dictionary<string,bool>();

           m_MakefileWriter.WriteLine();
           m_MakefileWriter.WriteLine("cmake_minimum_required(VERSION 2.6)");
           m_MakefileWriter.WriteLine();

           string strModuleName = Path.GetFileNameWithoutExtension(this.m_SlnFullPath);

           m_MakefileWriter.WriteLine("PROJECT( {0} )", strModuleName);
           m_MakefileWriter.WriteLine();

            foreach (KeyValuePair<string, string> pairProjectNamePath in this.m_ProjNamePath)
            {
                try
                {
                    string strProjectName = pairProjectNamePath.Key;
                    Console.WriteLine(".. Processing {0} ..", strProjectName);
                    m_MakefileWriter.WriteLine("#################################");
                    m_MakefileWriter.WriteLine("#### Target: {0} ####", strProjectName);
                    m_MakefileWriter.WriteLine("#################################");

                    XmlDocument doc = new XmlDocument();
                    doc.Load(Path.GetDirectoryName(this.m_SlnFullPath) + "/" + pairProjectNamePath.Value);
                    bProjectBuildRuleSuccess[strProjectName] = GenerateCMakeForProject(strProjectName, doc);
                }
                catch (Exception ex)
                {
                    Console.WriteLine("{0}: {1}", pairProjectNamePath.Value, ex.Message);
                }
            }


            Console.WriteLine("..Generating Dependency information..");
            m_MakefileWriter.WriteLine("#################################");
            m_MakefileWriter.WriteLine("#Link the targets with their Depenedency Libraries");
            m_MakefileWriter.WriteLine("#################################");

            foreach (KeyValuePair<string, string> pairProjectNamePath in this.m_ProjNamePath)
            {
                string strProjectName = pairProjectNamePath.Key;

                if (bProjectBuildRuleSuccess[strProjectName] == false) continue;

                string strDependencies = "";

                // Link against the dependencies
                foreach (string Dependency in this.m_ProjDependencies[strProjectName])
                {
                    //todo: use the real target name given the project name
                   strDependencies += (Dependency + " "); // todo: Use the real target name based on this project name
                   //strDependencies += (" ${" + Dependency + "_Dependencies} "); // Also link against the dependencie's dependencies
                }
               strDependencies += (" ${" + strProjectName + "_Librarian}"); // Also link the explicit librarian libs

                m_MakefileWriter.WriteLine("SET( {0} )", strProjectName + "_Dependencies " + strDependencies);
                m_MakefileWriter.WriteLine("target_link_libraries({0} {1})", strProjectName, " ${" + strProjectName + "_Dependencies}");
                m_MakefileWriter.WriteLine();
            }

            m_MakefileWriter.Close () ;

        }


        /// <summary>
        /// Generates Makefile, in case parseVcproj is true .vcproj parsing required
        /// </summary>
        /// <param name="parseVcproj">flag indicates if vcproj files should be parsed or not</param>
        public void GenerateMakefile (bool parseVcproj)
        {
            iPrintDefaultTargetRule  (         ) ;  //PHONY:all          
            iPrintTargetRules        (         ) ;  //PHONY:target
            iPrintClenOrDependsRules ("clean"  ) ;  //PHONY:clean
            iPrintClenOrDependsRules ("depends") ;  //PHONY:depends

            if (true == parseVcproj)
            {
                iSendToParseVcprojFiles  () ;
            }
            
            m_MakefileWriter.Close () ;
        }

        /// <summary>
        /// Initialize dictionaries with projects names, pathes and makefile names
        /// from previous parsing of .vcproj files list
        /// </summary>        
        public void InitDictionaries(Dictionary<string, string> projectNameGuid, string[] mainProjectDependencies)
        {
            string  makeFileName = "" ;
            int     numOfProject = 0  ;
            string  mainProjectFullPath = "";
           
            //Key = Guid number, Value = ProjectName
            m_ProjGuidName = projectNameGuid;

            Dictionary<string, string>.ValueCollection projectCollection= projectNameGuid.Values;

            foreach (string projectFullPath in projectCollection)
            {
                string projectName = Path.GetFileNameWithoutExtension(projectFullPath);

                if (0 != numOfProject)
                {
                    //Key = ProjectName , Value = ProjectFullPath
                    m_ProjNamePath.Add(projectName , projectFullPath);

                    //replace .vcproj extention to .mak for makefile name                    
                    makeFileName = projectFullPath.Replace(".vcproj", ".mak");  
                    makeFileName = makeFileName.Replace("\\", "/");

                    //Key = ProjectName , Value = MakeFileName
                    m_ProjMakFilePath.Add(projectName, makeFileName);

                    string[] dependencies = new string[0];
                    m_ProjDependencies.Add(projectName, dependencies);
                }
                else
                {
                    mainProjectFullPath = projectFullPath;

                    //Key = ProjectName , Value = ProjectFullPath
                    m_ProjNamePath.Add(projectName, projectFullPath);

                    //replace .vcproj extention to .mak for makefile name 
                    makeFileName = projectFullPath.Replace(".vcproj", ".mak");
                    makeFileName = makeFileName.Replace("\\", "/");

                    //Key = ProjectName , Value = MakeFileName
                    m_ProjMakFilePath.Add(projectName, makeFileName);

                    m_ProjDependencies.Add(projectName, mainProjectDependencies);
                }
                numOfProject++;
            }           
                                                     
            m_ProjPathCollection = m_ProjNamePath.Keys;            
        }

        /****************************************************************/
        #endregion /* Public Operations */

        #region Access Operations
        /****************************************************************/
        public Dictionary<string, string>.KeyCollection ProjectPathCollection  
        { 
            get {return m_ProjPathCollection;}
        }
        /****************************************************************/
        #endregion /* Access Operations */
    }
}
