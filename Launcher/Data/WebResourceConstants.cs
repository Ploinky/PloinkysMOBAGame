using System;
using System.CodeDom;
using System.Collections.Generic;
using System.Linq;
using System.Net.NetworkInformation;
using System.Text;
using System.Threading.Tasks;
using static System.Net.WebRequestMethods;

namespace Launcher.Data
{
    public class WebResourceConstants
    {
        public static readonly String LAUNCHER_VERSION_FILE = "https://www.dropbox.com/s/mgk8aj6g1spdcz1/pmg_launcher_version?dl=1";
        public static readonly String GAME_VERSION_FILE = "https://www.dropbox.com/s/wz1vcpjbtmg1epy/pmg_version?dl=1";
        public static readonly String GAME_DATA = "https://www.dropbox.com/s/bdsp8enqu161vjw/Client.zip?dl=1";
        public static readonly String LAUNCHER_DATA = "https://www.dropbox.com/s/18826n37pfnqyiz/Launcher.zip?dl=1";
    }
}
