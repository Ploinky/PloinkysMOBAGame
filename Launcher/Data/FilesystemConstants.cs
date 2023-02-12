using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Updater.Data
{
    public class FilesystemConstants
    {
        public static readonly String ROOTPATH = Directory.GetCurrentDirectory();
        public static readonly String LAUNCHER_EXE = Path.Combine(ROOTPATH, "Launcher.exe");
        public static readonly String LAUNCHER_ZIP = Path.Combine(ROOTPATH, "Launcher.zip");
        public static readonly String TEMP_LAUNCHER_EXE = Path.Combine(ROOTPATH, "_Launcher.exe");
        public static readonly String GAME_EXE = Path.Combine(ROOTPATH, "Client.exe");
        public static readonly String VERSION_FILE= Path.Combine(ROOTPATH, "pmg_version");
        public static readonly String LAUNCHER_VERSION_FILE = Path.Combine(ROOTPATH, "pmg_launcher_version");
        public static readonly String GAME_ZIP = Path.Combine(ROOTPATH, "PMG.zip");
    }
}
