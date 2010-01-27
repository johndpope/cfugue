using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.VisualStudio.VCProjectEngine;

namespace sln2mak
{
    class VcTargetType
    {
        #region Members
        /****************************************************************/
        private bool    exeflag ;
        private bool    dllflag ;
        private bool    libflag ;
        private string  confName;
        /****************************************************************/
        #endregion /* Members */

        #region LifeCycle
        /****************************************************************/
        public VcTargetType (VCConfiguration config)
        {
            exeflag = false ;
            dllflag = false ;
            libflag = false ;

            confName = config.ConfigurationName;
            iSetTargetFlags(config.ConfigurationType);
        }
        /****************************************************************/
        #endregion /* LifeCycle */

        #region Private Operations
        /****************************************************************/
        /// <summary>
        /// Set true to flag that responses target type
        /// </summary>
        /// <param name="configType"></param>
        private void iSetTargetFlags(ConfigurationTypes configType)
        {
            switch (configType)//dynamiclib or staticlib and etc.
            {
                case (ConfigurationTypes.typeApplication):
                    exeflag = true;
                    dllflag = false;
                    libflag = false;
                    break;
                case (ConfigurationTypes.typeDynamicLibrary):
                    exeflag = false;
                    dllflag = true;
                    libflag = false;
                    break;
                case (ConfigurationTypes.typeStaticLibrary):
                    exeflag = false;
                    dllflag = false;
                    libflag = true;
                    break;
                case (ConfigurationTypes.typeGeneric):
                    //TODO: check what is typeGeneric
                    exeflag = false;
                    dllflag = false;
                    libflag = false;
                    break;
                default://ConfigurationTypes.typeUnknown
                    //TODO: throw exception or exit with error
                    exeflag = false;
                    dllflag = false;
                    libflag = false;
                    break;
            }
        }

        /****************************************************************/
        #endregion /* Private Operations */

        #region Access Region
        /****************************************************************/
        public bool ExeFlag
        { 
            get { return exeflag; } 
        }
        public bool DllFlag
        {
            get { return dllflag; }
        }
        public bool LibFlag
        {
            get { return libflag; }
        }
        public string ConfigName 
        {
            get { return confName; } 
        }
        /****************************************************************/
        #endregion /* Access Region */
    }
}
