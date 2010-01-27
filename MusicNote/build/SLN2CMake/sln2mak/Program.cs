using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;
using System.IO;

namespace sln2mak
{
    class Program
    {
        static TextWriter  m_ErrorWriter = Console.Error;


        static void Main(string[] args)
        {                        
            String      m_ProjectName ;
            String      m_InFileName="";            


            Console.WriteLine("sln2mak.exe - Generates a Makefile from a .sln/.vcproj file .\n \u00a9 Maria Adamsky (www.EFC.CO.IL)\n");

            if(args.Length < 1)
            {                
                Program.Usage();

                Environment.Exit(1);
            }
            #region ARG_FORMAT -l proj_name project.sln
            /****************************************************************/
            else if (args[0] == "-l") //format: -l main-project-name workspace-name.sln
	        {
                if (args.Length < 3)
		        {
                    m_ErrorWriter.WriteLine("Wrong arguments format!\n");
                    Environment.Exit(1);
		        }
		        else 
		        {
                    m_ProjectName    = args[1] ; //args[2] is main-project-name
			        m_InFileName     = args[2] ; //args[3] is workspace-name.sln
			        if(Parser.SlnExtentionRegex.Match(m_InFileName).Success) 
			        {
                        Parser.ParseSln(m_ProjectName, m_InFileName);						        
			        } 			
			        else 
			        {
                        Program.Usage();

                        Environment.Exit(1);
			        }
                }
            }
            /****************************************************************/
            #endregion /*ARG_FORMAT -l proj_name project.sln*/
            #region ARG_FORMAT  project.sln
            /****************************************************************/
            else if (Parser.SlnExtentionRegex.Match(args[0]).Success) //format:  workspace-name.sln
	        {
                m_InFileName = args[0]; 			                          //args[0] is workspace-name.vcproj		
                m_ProjectName = Path.GetFileNameWithoutExtension(m_InFileName) ; //main-project-name taken from workspace-name.sln		        			        


                Parser.ParseSln(m_ProjectName, m_InFileName);		
		        
            }
            /****************************************************************/
            #endregion /*ARG_FORMAT  project.sln*/
            #region ARG_FORMAT  list of .vcproj files -d additional_dependencies
            /****************************************************************/
            else if (Parser.VcprojExtentionRegex.Match(args[0]).Success)     // format:  list of project-name.vcproj
            {
                //int numOfProjects = 0;
                List<string> mainProjectDependencies = new List<string>();  
                List<string> projectsList            = new List<string>();

                //Read all .vcproj list into projectsList and if '-d' exists init mainProjectDependencies list too
                for (int i = 0; i < args.Length; i++)
                {
                    if (Parser.VcprojExtentionRegex.Match(args[i]).Success)
                    {
                        projectsList.Add(args[i]);
                    }
                    if (args[i].Equals("-d"))
                    {
                        for (int d = i + 1; d < args.Length; d++)
                        {
                            mainProjectDependencies.Add(args[d]);
                        }
                        break;
                    }
                }

                for (int p = 0; p < projectsList.Count; p++)
                {
                    string[]    dependencies    ;
                    string      makefilePath = projectsList[p].Replace(".vcproj", ".mak");

                    //The first project in list is main one, that's why all dependencies after '-d' relies to it
                    if (0 == p)
                    {
                        //dependencies = new string[mainProjectDependencies.Count];
                        dependencies = mainProjectDependencies.ToArray();
                    }
                    else
                    {
                        dependencies = new string[0];
                    }
                    //Create VcProjInfo object and call ParseVcproj, that generates projectsList[p].mak file
                    VcProjInfo vcProjInfo = new VcProjInfo(projectsList[p], makefilePath, dependencies);
                    vcProjInfo.ParseVcproj();
                }

                
                //create makefile for all projects in list passed as arguments
                Parser.CreateMakefile(projectsList, mainProjectDependencies.ToArray());       
            }
            /****************************************************************/
            #endregion /* ARG_FORMAT  list of .vcproj files -d additional_dependencies */
            else
            {
                Program.Usage();
                
                Environment.Exit(1);
            }	
        }

        public static void Usage()
        {
            m_ErrorWriter.WriteLine("\t\tWRONG ARGUMENTS FORMAT!\n");
            m_ErrorWriter.WriteLine("********************************************************************************************\n");
            m_ErrorWriter.WriteLine("There are three optional argument formats:\n"                    ) ;
            m_ErrorWriter.WriteLine("1. Only one argument is solution file name.\n"                   ) ;
            m_ErrorWriter.WriteLine("In this case application assumes that main project name\n"       ) ;
            m_ErrorWriter.WriteLine("similar to solution file name without file extention, example:\n") ;
            m_ErrorWriter.WriteLine("\tsln2mak [Solution_File_Name].sln \n"                           ) ;
            m_ErrorWriter.WriteLine("********************************************************************************************\n");

            m_ErrorWriter.WriteLine("2. Three arguments.\n"                                           ) ;
            m_ErrorWriter.WriteLine("First argument is '-l' flag that signs 'leader' project name,\n" ) ;
            m_ErrorWriter.WriteLine("second argument is leader(main) project name,\n"                 ) ;
            m_ErrorWriter.WriteLine("and third argument is is solution file name.\n"                  ) ;
            m_ErrorWriter.WriteLine("\tsln2mak -l [Main_Project_Name] [Solution_File_Name].sln \n"    ) ;

            m_ErrorWriter.WriteLine("********************************************************************************************\n");

            m_ErrorWriter.WriteLine("3. More then three arguments.\n"                                    ) ;
            m_ErrorWriter.WriteLine("First arguments are list of .vcproj files, when first one in list\n") ;
            m_ErrorWriter.WriteLine("is a main project.\n"                                               ) ;
            m_ErrorWriter.WriteLine("The list of .vcproj files followed by '-d' flag that \n"            ) ;
            m_ErrorWriter.WriteLine("signs start of additional dependencies list.\n"                     ) ;
            m_ErrorWriter.WriteLine("Additional dependencies list is a list of additional libraries \n"  ) ;
            m_ErrorWriter.WriteLine("for main project linkage.\n"                                        ) ;
            m_ErrorWriter.WriteLine("\tsln2mak [Main_Project_Name].vcproj [Project_Name_2].vcproj ... [Project_Name_n].vcproj -d [lib_Name_1] ... [lib_Name_n] \n");
            m_ErrorWriter.WriteLine("********************************************************************************************\n");
        }        
    }
}
