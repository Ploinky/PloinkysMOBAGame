using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Updater.Data;

namespace Updater.Views
{
    /// <summary>
    /// Interaction logic for Dashboard.xaml
    /// </summary>
    public partial class Dashboard : Page
    {
        public Dashboard()
        {
            InitializeComponent();
        }

        private void ConnectButton_Click(object sender, RoutedEventArgs e)
        {
            ProcessStartInfo startInfo = new ProcessStartInfo(FilesystemConstants.GAME_EXE);
            startInfo.ArgumentList.Add("-connect");
            startInfo.ArgumentList.Add(TextBox_IP.Text);

            Process.Start(startInfo);

            Window main = Application.Current.MainWindow;
            if (main != null)
            {
                main.Close();
            }
        }
    }
}
