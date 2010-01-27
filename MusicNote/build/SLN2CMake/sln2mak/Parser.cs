using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;
using System.IO;
using System.Xml;
using Microsoft.VisualStudio.VCProjectEngine;

namespace sln2mak
{
    class Parser
    {
        #region Members
        /****************************************************************/
        private static Regex     m_ProjectGuid     ;
        private static Regex     m_SlnExtention    ;
        private static Regex     m_VcprojExtention ;
        private static Regex     m_ProjectRegex    ;
        /****************************************************************/
        #endregion /* Members */

        #region LifeCycle
        /****************************************************************/
        static Parser()
        {
            m_ProjectGuid       = new Regex (@"ProjectGUID=""\{(.*)\}"""        ) ;
            m_SlnExtention      = new Regex (@"(.*)(.[Ss][Ll][Nn])$"            ) ;
            m_VcprojExtention   = new Regex (@"(.*)(.[Vv][Cc][Pp][Rr][Oo][Jj])$") ;
            m_ProjectRegex      = new Regex (@"Project\(""\{(.*)\}""\) = ""(.*)"", ""(.*)"", ""\{(.*)\}""") ;
        }
        /****************************************************************/
        #endregion /* LifeCycle */

        #region Access  Region
        /****************************************************************/
        public static Regex ProjectGuidRegex 
        {
            get { return m_ProjectGuid; }
        }

        public static Regex SlnExtentionRegex
        {
            get { return m_SlnExtention; }
        }

        public static Regex VcprojExtentionRegex
        {
            get { return m_VcprojExtention; }
        }

        public static Regex ProjectRegex
        {
            get { return m_ProjectRegex ; }
        }
        /*===================================================================*/
        #endregion /* Access  Region */

        #region Public Operations
        /*===================================================================*/
        /// <summary>
        /// After .vcproj files list parsed call to CreateMakefile 
        /// for Makefile generation
        /// </summary>
        public static void CreateMakefile  (List<string> projectsList, 
                                            string[]     mainProjectDependencies  )
        {
            StreamReader                sr;
            Match                       matchProjInfo;
            string                      line;
            Dictionary<string, string>  projectNameGuid = new Dictionary<string, string>();


            //Initialize dictionary with project guid and name
            foreach(string project in projectsList)
            {
                sr = new StreamReader(project);
                while ((line = sr.ReadLine()) != null)
                {
                    matchProjInfo = ProjectGuidRegex.Match(line);
                    if (matchProjInfo.Success)
                    {
                        projectNameGuid.Add(matchProjInfo.Groups[1].Value, project);
                    }
                }

            }

            //Instantinate VcSlnInfo object with main project name
            VcSlnInfo slnInfo = new VcSlnInfo(projectsList[0]);
            //Initialize all dictioanries for VcSlnInfo object
            slnInfo.InitDictionaries(projectNameGuid, mainProjectDependencies);
            slnInfo.GenerateMakefile(false);
            
        }

        /// <summary>
        /// Generate makefile after object slnInfo with main project name and 
        /// .sln file name(full path) instantinated 
        /// </summary>
        /// <param name="projName">main project name </param>
        /// <param name="slnFName">.sln file name(full path)</param>
        public static void ParseSln (string projName, 
                                     string  slnFName    )
        {                                    
           
            VcSlnInfo slnInfo = new VcSlnInfo(projName, slnFName);

            slnInfo.GenerateMakefile();                    
        }
        /*===================================================================*/
        #endregion /* Public Operations */          
    }
}
