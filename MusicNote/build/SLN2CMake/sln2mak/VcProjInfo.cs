using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using Microsoft.VisualStudio.VCProjectEngine;

namespace sln2mak
{
    class VcProjInfo
    {
        #region Members
        /****************************************************************/
        private     VCProject                           m_VcProj                  ;
        private     string                              m_MakeFileName            ;
        private     string[]                            m_VcProjDependencies      ;
        private     StreamWriter                        m_MakFWriter              ;
        private     string                              m_ProjectName             ;
        private     IVCCollection                       m_ConfigCollection        ;
        private     string                              m_PlatfromName            ;
        private     IVCCollection                       m_FileFilterCollection    ;
        private     Dictionary<string, VcTargetType>    m_TargetDictionary        ;
        private     int                                 m_NumOfConfigs            ;
        private     string                              m_SRCGroups               ;
        /****************************************************************/
        #endregion /* Members */

        #region LifeCycle
        /****************************************************************/
        public VcProjInfo(string vcProjFile, string outFile, string[] projDependencies)
        {
            Console.WriteLine("Going to read project file '{0}'", vcProjFile);
            Console.WriteLine("aiming to write result to file '{0}'", outFile);
            if (projDependencies.GetLength(0) <= 0){
                Console.WriteLine("With no dependencies."); 
            }
            else
            {
                Console.WriteLine("With dependencies:");
                foreach (String s in projDependencies)
                {
                    Console.WriteLine("\t" + s);
                }
                //projDependencies.ToString());
            }
            Console.WriteLine(""); // to visually separate output.
            
            //Init project name and .mak file name
            m_ProjectName = Path.GetFileNameWithoutExtension(vcProjFile);
            m_MakeFileName = outFile;
            m_VcProjDependencies = projDependencies;

            VCProjectEngine vcprojEngine = new VCProjectEngineObject();

            try
            {
                //Init VCProject vcProj object
                m_VcProj = (VCProject)vcprojEngine.LoadProject(vcProjFile);
            }
            catch(Exception ex)
            {
                Console.WriteLine("Some errors occurred during project loading: {0}\n\n\n", ex.Message);
                Console.WriteLine("This tool knows to convert Visual Studio 2005 projects only\n");
                Console.WriteLine("For converting Visual Studio 2008 projects, compile this tool on VS2008 \n");
                Console.WriteLine("add Microsoft.VisualStudio.VCProjectEngine9.0.0 reference\n");
            }

            //Init vcproj configurations list 
            m_ConfigCollection = (IVCCollection)m_VcProj.Configurations;

            //Init targets dictionary (Key is configuration name, Value is VcTargetType object
            m_TargetDictionary = new Dictionary<string, VcTargetType>();
            foreach (VCConfiguration config in m_ConfigCollection)
            {
                m_TargetDictionary.Add(config.ConfigurationName, new VcTargetType(config));
            }

            //Init number of vcProj configurations
            m_NumOfConfigs = m_ConfigCollection.Count;

            //Init platform name of vcProj
            IVCCollection platformCollection = (IVCCollection)m_VcProj.Platforms;
            VCPlatform platform = (VCPlatform)platformCollection.Item(1);
            m_PlatfromName = platform.Name;

            //Init Filters collection
            m_FileFilterCollection = (IVCCollection)m_VcProj.Filters;

            Console.WriteLine("Creating file " + m_MakeFileName + "...");

            //Open StreamWriter for writing into makFileName
            m_MakFWriter = new StreamWriter(m_MakeFileName);

            m_SRCGroups = "SRCS=";
        }

        /****************************************************************/
        #endregion /* LifeCycle */
        
        #region Public Operations
        /****************************************************************/
        public void ParseVcproj()
        {                 
            //this specifies the name of the 'rm -f' or equivalent command
            string rm = "rm -f -v";

            //this specifies the default name for the dependencies file
            string dependencies = ".dependencies";
            string cflags       = "";
            string ldflags      = "";
            string libs         = "";
            

            m_MakFWriter.WriteLine("# Makefile - {0}", m_ProjectName);
            m_MakFWriter.WriteLine();

            //get the project name            
            dependencies = m_ProjectName + ".dep";

            //catch the default configuration definition = first configuration in vcproj configurations
            m_MakFWriter.WriteLine("ifndef CFG");
            m_MakFWriter.WriteLine("CFG={0}", ((VCConfiguration)m_ConfigCollection.Item(1)).ConfigurationName);
            m_MakFWriter.WriteLine("endif");

            //print the C++ compiler definition
            m_MakFWriter.WriteLine("CC=gcc");

            if (m_PlatfromName.Equals("Win32"))
            {
                m_MakFWriter.WriteLine("CFLAGS=-m32 -fpic");
            }
            else
            {
                m_MakFWriter.WriteLine("CFLAGS=-m64 -fpic");
            }
            m_MakFWriter.WriteLine("CXX=g++");
            m_MakFWriter.WriteLine("CXXFLAGS=$(CFLAGS)");

            #region Config Parsing
            /**********************************/
            foreach (VCConfiguration config in m_ConfigCollection)
            {
                //print else for "$(CFG)" section in case it is not first config in makefile
                //if (0 != m_NumOfConfigs)
                //{
                //    m_MakFWriter.WriteLine("else");
                //} //TODO:check this issue

                //print ifeq "$(CFG)" "CONFIG_NAME"
                m_MakFWriter.WriteLine("ifeq \"$(CFG)\" \"{0}\"", config.ConfigurationName);

                IVCCollection toolsCollection = (IVCCollection)config.Tools;

                #region VCCLCompilerTool
                /**********************************/
                VCCLCompilerTool compTool = (VCCLCompilerTool)toolsCollection.Item("VCCLCompilerTool");

                if (null != compTool)
                {
                    cflags = iCreateCflags(compTool);
                

                    //add special include path for VTOC files
                    for (int i = 0; i < m_VcProjDependencies.Length; i++)
                    {
                        if (m_VcProjDependencies[i].Equals("v_to_c"))
                        {
                            cflags += " -I VTOC/share/include";
                        }
                    }

                    m_MakFWriter.WriteLine("CFLAGS+={0}", cflags);
                }
                /**********************************/
                #endregion VCCLCompilerTool

                #region VCLinkerTool
                /**********************************/
                VCLinkerTool lnkTool = (VCLinkerTool)toolsCollection.Item("VCLinkerTool");
                if (null != lnkTool)
                {
                    if ((m_TargetDictionary[config.ConfigurationName].ExeFlag) ||
                        (m_TargetDictionary[config.ConfigurationName].DllFlag))
                    {
                        m_MakFWriter.WriteLine("LD=$(CXX) $(CXXFLAGS)");
                    }

                    m_MakFWriter.WriteLine("LDFLAGS=");

                    libs = iCreateLibs(lnkTool);
                    ldflags = iCreateLdflags(lnkTool);

                    m_MakFWriter.WriteLine("LDFLAGS+={0}", ldflags);
                    if (m_TargetDictionary[config.ConfigurationName].ExeFlag)
                    {
                        m_MakFWriter.WriteLine("LIBS+={0}", libs);
                    }
                    if (m_TargetDictionary[config.ConfigurationName].DllFlag)
                    {
                        m_MakFWriter.WriteLine("LIBS+= -shared {0}", libs);
                    }
                }

                /**********************************/
                #endregion VCLinkerTool

                #region VCLibrarianTool
                /**********************************/
                VCLibrarianTool libTool = (VCLibrarianTool)toolsCollection.Item("VCLibrarianTool");

                if (null != libTool)
                {
                    m_MakFWriter.WriteLine("AR=ar");
                    m_MakFWriter.WriteLine("ARFLAGS=rus");
                }
                /**********************************/
                #endregion VCLibrarianTool

                //print target for each configuration
                iPrintTARGET2Makefile(config.ConfigurationName);
            }

            //close every ifeq "$(CFG)" with endif
            for (int i = 0; i < m_NumOfConfigs; i++)
            {
                m_MakFWriter.WriteLine("endif");
            }
            /**********************************/
            #endregion Config Parsing

            //print default target for first configuration in configCollection
            iPrintTARGET2Makefile(((VCConfiguration)m_ConfigCollection.Item(1)).ConfigurationName);

            //print the default target and the suffix rules
            m_MakFWriter.WriteLine(".PHONY: all");
            m_MakFWriter.WriteLine("all: $(TARGET)");
            m_MakFWriter.WriteLine("");

            m_MakFWriter.WriteLine("%.o: %.c");
            m_MakFWriter.WriteLine("\t$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<");
            m_MakFWriter.WriteLine("");

            m_MakFWriter.WriteLine("%.o: %.cc");
            m_MakFWriter.WriteLine("\t$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $<");
            m_MakFWriter.WriteLine("");

            m_MakFWriter.WriteLine("%.o: %.cpp");            
            m_MakFWriter.WriteLine("\t$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $<");
            m_MakFWriter.WriteLine("");

            m_MakFWriter.WriteLine("%.o: %.cxx");
            m_MakFWriter.WriteLine("\t$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $<");
            m_MakFWriter.WriteLine("");

            m_MakFWriter.WriteLine("%.res: %.rc");
            m_MakFWriter.WriteLine("\t$(RC) $(CPPFLAGS) -o $@ -i $<");
            m_MakFWriter.WriteLine("");

            #region Sources
            /**********************************/
            iPrintGroupsAndSources();
            /**********************************/
            #endregion Sources

            //define the objects automatically from the sources in the Makefile
            m_MakFWriter.WriteLine("OBJS=$(patsubst %.rc,%.res,$(patsubst %.cxx,%.o,$(patsubst %.cpp,%.o,$(patsubst %.cc,%.o,$(patsubst %.c,%.o,$(filter %.c %.cc %.cpp %.cxx %.rc,$(SRCS)))))))");
            m_MakFWriter.WriteLine("");

            //print the target rule, according to the type of deafult target = target of first configuration in vcproj configurations list
            m_MakFWriter.WriteLine("$(TARGET): $(OBJS)");
            VcTargetType target = m_TargetDictionary[((VCConfiguration)m_ConfigCollection.Item(1)).ConfigurationName];
            if (target.ExeFlag)
            {
                m_MakFWriter.WriteLine("\t$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)");
            }
            if (target.DllFlag)
            {
                m_MakFWriter.WriteLine("\t$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)");
            }
            if (target.LibFlag)
            {
                m_MakFWriter.WriteLine("\t$(AR) $(ARFLAGS) $@ $(OBJS)");
            }
            m_MakFWriter.WriteLine("");

            // print the 'clean' target rule
            m_MakFWriter.WriteLine(".PHONY: clean");
            m_MakFWriter.WriteLine("clean:");
            m_MakFWriter.WriteLine("\t-{0} $(OBJS) $(TARGET) {1}", rm, dependencies);
            m_MakFWriter.WriteLine("");

            // print the 'depends' target rule for automatic dependencies generation
            m_MakFWriter.WriteLine(".PHONY: depends");
            m_MakFWriter.WriteLine("depends:");
            m_MakFWriter.WriteLine("\t-$(CXX) $(CXXFLAGS) $(CPPFLAGS) -MM $(filter %.c %.cc %.cpp %.cxx,$(SRCS)) > {0}", dependencies);
            m_MakFWriter.WriteLine("");

            m_MakFWriter.WriteLine("-include {0}", dependencies);
            m_MakFWriter.WriteLine("");

            m_MakFWriter.Close();

            iFixMakfile();
        }
        /****************************************************************/
        #endregion /* Public Operations */

        #region Private Operations
        /****************************************************************/
        /// <summary>
        /// Print TARGET to .mak file accordingly to Type within specific config name
        /// </summary>        
        /// <param name="configName"></param>
        private void iPrintTARGET2Makefile(string configName)
        {
            m_MakFWriter.WriteLine("ifndef TARGET");
            if (m_TargetDictionary[configName].ExeFlag)
            {
                m_MakFWriter.WriteLine("TARGET={0}.exe", m_ProjectName);
            }
            else if (m_TargetDictionary[configName].DllFlag)
            {
                m_MakFWriter.WriteLine("TARGET={0}.dll", m_ProjectName);
            }
            else if (m_TargetDictionary[configName].LibFlag)
            {
                m_MakFWriter.WriteLine("TARGET=lib{0}.a", m_ProjectName);
            }
            m_MakFWriter.WriteLine("endif");
        }

        /// <summary>
        /// Create cglags string with all compiler options
        /// </summary>
        /// <param name="compTool">compiler tool</param>
        /// <returns>cflags</returns>
        private string iCreateCflags(VCCLCompilerTool compTool)
        {
            string option = "";
            string cflags = "";


            //Disable Language Extensions
            option = (compTool.DisableLanguageExtensions) ? "-ansi" : "";
            cflags += " " + option;

            //Warning Level
            switch (compTool.WarningLevel)
            {
                case (warningLevelOption.warningLevel_0):
                    //Turns off all warning messages
                    option = "-w";
                    break;
                case (warningLevelOption.warningLevel_1):
                case (warningLevelOption.warningLevel_2):
                case (warningLevelOption.warningLevel_3):
                    //Warning Level
                    option = "-W";
                    break;
                case (warningLevelOption.warningLevel_4):
                    //Highest Warning Level
                    option = "-Wall";
                    break;
            }
            cflags += " " + option;

            //Warnings As Errors
            option = (compTool.WarnAsError) ? "-Werror" : "";
            cflags += " " + option;

            //Add optimization to CFLAGS
            switch (compTool.Optimization)
            {
                case (optimizeOption.optimizeDisabled):
                    //Disable Optimizations
                    option = "-O0";
                    break;
                case (optimizeOption.optimizeMinSpace):
                    //Minimize Size
                    option = "-Os";
                    break;
                case (optimizeOption.optimizeMaxSpeed):
                    //Maximize Speed
                    option = "-O2";
                    break;
            }
            cflags += " " + option;

            //Enable Exception Handling
            switch (compTool.ExceptionHandling)
            {
                case (cppExceptionHandling.cppExceptionHandlingYes):
                case (cppExceptionHandling.cppExceptionHandlingYesWithSEH):
                    option = "-fexceptions";
                    break;
                case (cppExceptionHandling.cppExceptionHandlingNo):
                    option = "";
                    break;
            }
            cflags += " " + option;

            //Debug Info
            switch (compTool.DebugInformationFormat)
            {
                case (debugOption.debugEditAndContinue):
                case (debugOption.debugEnabled):
                case (debugOption.debugOldStyleInfo):
                    option = "-g";
                    break;
                case (debugOption.debugDisabled):
                    option = "";
                    break;
            }
            cflags += " " + option;


            //Frame-Pointer Omission
            option = (compTool.OmitFramePointers) ? "-fomit-frame-pointer" : "";
            cflags += " " + option;

            //Add inline function flags to CFLAGS
            switch (compTool.InlineFunctionExpansion)
            {
                case (inlineExpansionOption.expandDisable):// /Ob0
                    //Disables inline Expansion 
                    option = "-fno-inline";
                    break;
                case (inlineExpansionOption.expandOnlyInline):// /Ob1
                    //In-line Function Expansion
                    option = "";
                    break;
                case (inlineExpansionOption.expandAnySuitable):// /Ob2
                    //auto In-line Function Expansion
                    option = "-finline-functions";
                    break;
            }
            cflags += " " + option;

            switch (compTool.StructMemberAlignment)
            {
                case (structMemberAlignOption.alignSingleByte):
                    //Packs structures on 1-byte boundaries
                    option = "-fpack-struct";
                    break;
                default:
                    option = "";
                    break;
            }
            cflags += " " + option;

            //Add preprocessor definitions to CFLAGS
            option = compTool.PreprocessorDefinitions;
            if (option != null)
            {
                option = option.Replace(";", " -D");
                option = "-D" + option;
            }
            else
                option = "";
            
            cflags += " " + option;

            //Add include directories to CFLAGS
            option = compTool.AdditionalIncludeDirectories;
            if (option != null)
            {
                option = option.Replace(",", " -I ");
                option = " -I " + option;
            }
            else
                option = "";
           
            cflags += " " + option;



            return (cflags);
        }

        /// <summary>
        /// Create libs from main project dependencies and linker additional dependencies 
        /// </summary>
        /// <param name="lnkTool">linker tool</param>        
        /// <returns>libs</returns>
        private string iCreateLibs(VCLinkerTool lnkTool)
        {
            string libs = "";


            libs = lnkTool.AdditionalDependencies;
            if (libs != null)
            {
                libs = libs.Replace(".lib", "");
                libs = libs.Replace(" ", " -l");
                libs = "-l" + libs;
            }
            else
            {
                libs = "";
            }
			
            for (int i = 0; i < m_VcProjDependencies.Length; i++) 
            {
                if (m_VcProjDependencies[i].Equals("v_to_c"))
                {
                    libs += " -LVTOC/i686-RHEL3-gcc-3.2.3/lib -lvtoc ";
                }                
            }
            //add stdc++ and m libs
            libs += "-lstdc++ -lm";

            return (libs);
        }

        /// <summary>
        /// Create ldflags from all linker options
        /// </summary>
        /// <param name="lnkTool">linker tool</param>
        /// <returns>ldflags</returns>
        private string iCreateLdflags(VCLinkerTool lnkTool)
        {
            string ldflags = "";
            string option = "";


            //Additional Libpath
            option = lnkTool.AdditionalLibraryDirectories;
            if (option != null)
            {
                option = option.Replace(";", " -L");
                option = "-L" + option;
            }
            else
                option = "";
            ldflags += " " + option;

            //Generate Mapfile
            option = (lnkTool.GenerateMapFile) ? "-map" : "";
            if (!(option.Equals("")))
            {
                option += lnkTool.MapFileName;
                ldflags += " " + option;
            }

            return (ldflags);
        }

        /// <summary>
        /// Print to .mak file all sources within its filter(group) and then print all groups
        /// </summary>        
        private void iPrintGroupsAndSources()
        {
            //Filter name is a virtual directory for grouping files in vcproj
            foreach (VCFilter filter in m_FileFilterCollection)
            {
                iPrintRecursiveGroupsAndSources(filter);
            }

            m_MakFWriter.WriteLine("{0}", m_SRCGroups);
            m_MakFWriter.WriteLine("");
        }

        /// <summary>
        /// Recursive filter expand and print sources
        /// </summary>
        /// <param name="filter"></param>
        private void iPrintRecursiveGroupsAndSources(VCFilter filter)
        {
            IVCCollection filterCollection = (IVCCollection)filter.Filters;

            if (0 == filterCollection.Count)
            {
                //print the sources files groups
                string group = filter.Name.Replace(" ", "_");
                group = group.ToUpper();
                m_MakFWriter.WriteLine("{0}= \\", group);

                m_SRCGroups += "$(" + group + ") ";
                IVCCollection filesCollection = (IVCCollection)filter.Files;

                int fileCntr = 0;

                foreach (VCFile vcFile in filesCollection)
                {
                    string fileName = vcFile.RelativePath.Replace("\\", "/");                    

                    if (fileCntr < filesCollection.Count - 1)
                    {
                        m_MakFWriter.WriteLine("\t{0} \\", fileName);//print slash as in macro
                    }
                    else
                    {
                        m_MakFWriter.WriteLine("\t{0}", fileName);//in case of last file in group no slash needed
                    }
                    fileCntr++;
                }
                m_MakFWriter.WriteLine("");
            }
            else
            {
                foreach (VCFilter f in filterCollection)
                {
                    iPrintRecursiveGroupsAndSources(f);
                }
            }
        }

        /// <summary>
        /// Fix windows extentions from makefiles
        /// </summary>
        private void iFixMakfile()
        {
            StreamReader sr;
            string line = "";
            FileInfo fi = new FileInfo(m_MakeFileName);
            FileInfo fi_old = new FileInfo(m_MakeFileName + ".old");


            fi_old.Delete();
            fi.MoveTo(m_MakeFileName + ".old");

            sr = new StreamReader(m_MakeFileName + ".old");
            m_MakFWriter = new StreamWriter(m_MakeFileName);

            while ((line = sr.ReadLine()) != null)
            {
                line = line.Replace("-DWIN32", "");
                line = line.Replace("-Wall", "-Wall -W -Werror");
                line = line.Replace("-D_MBCS", "");
                line = line.Replace("-llibxml2", "-lxml2");
                line = line.Replace("-lkernel32", "");
                line = line.Replace("-luser32", "");
                line = line.Replace("-lnafxcwd", "");
                line = line.Replace("-lnafxcw", "");
                line = line.Replace("-lgdi32", "");
                line = line.Replace("-lwinspool", "");
                line = line.Replace("-lcomdlg32", "");
                line = line.Replace("-ladvapi32", "");
                line = line.Replace("-lshell32", "");
                line = line.Replace("-lole32", "");
                line = line.Replace("-loleaut32", "");
                line = line.Replace("-luuid", "");
                line = line.Replace("-lodbc32", "");
                line = line.Replace("-lodbccp32", "");
                line = line.Replace("-lnetapi32", "");
                line = line.Replace("-lcomctl32", "");
                line = line.Replace("-lwsock32", "");                
                line = line.Replace("-llibvtoc_vc6", "");
                line = line.Replace("-llibvtoc", "");
                line = line.Replace("-llmgr", "");
                line = line.Replace("nafxcwd ", "");
                line = line.Replace("netapi32", "");
                line = line.Replace("comctl32", "");
                line = line.Replace("wsock32", "");                
                line = line.Replace("lmgr", "");
                line = line.Replace("odbc32", "");
                line = line.Replace("odbccp32", "");
                line = line.Replace("Win32", "Linux");
                line = line.Replace("-D_MSC_VER", "");
                // ahmad added
                line = line.Replace("WIN32", "LINUX");

                m_MakFWriter.WriteLine(line);

            }
            m_MakFWriter.Close();
            sr.Close();
            fi_old.Delete();
        }
        /****************************************************************/
        #endregion /* Private Operations */
    }
}
