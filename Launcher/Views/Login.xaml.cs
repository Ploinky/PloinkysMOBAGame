using System;
using System.ComponentModel;
using System.IO.Compression;
using System.IO;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using Updater.Data;
using Launcher.Data;
using System.Diagnostics;
using System.Threading.Tasks;

namespace Updater.Views
{
    enum UpdaterState
    {
        READY,
        FAILED,
        DOWNLOADING_GAME,
        DOWNLOADING_UPDATE
    }

    /// <summary>
    /// Interaction logic for Login.xaml
    /// </summary>
    public partial class Login : Page
    {
        private UpdaterState _status;
        internal UpdaterState status
        {
            get => _status;

            set
            {
                _status = value;

                Dispatcher.Invoke(new Action(() =>
                {
                    switch (_status)
                    {
                        case UpdaterState.READY:
                            LoginButton.Content = "Play";
                            LoginButton.IsEnabled = true;
                            RegisterButton.IsEnabled = true;
                            break;
                        case UpdaterState.FAILED:
                            LoginButton.Content = "Update Failed, Retry";
                            LoginButton.IsEnabled = true;
                            RegisterButton.IsEnabled = false;
                            break;
                        case UpdaterState.DOWNLOADING_GAME:
                            LoginButton.Content = "Downloading Game";
                            LoginButton.IsEnabled = false;
                            RegisterButton.IsEnabled = false;
                            break;
                        case UpdaterState.DOWNLOADING_UPDATE:
                            LoginButton.Content = "Downloading Update";
                            LoginButton.IsEnabled = false;
                            RegisterButton.IsEnabled = false;
                            break;
                        default:
                            break;
                    }
                }));
            }
        }

        public Login()
        {
            InitializeComponent();
            Task.Run(() => CheckForLauncherUpdates());
        }

        private void CheckForLauncherUpdates()
        {
            if (File.Exists(FilesystemConstants.TEMP_LAUNCHER_EXE))
            {
                File.Delete(FilesystemConstants.TEMP_LAUNCHER_EXE);
            }

            WebClient webClient = new();
            Version onlineLauncherVersion = new(webClient.DownloadString(WebResourceConstants.LAUNCHER_VERSION_FILE));

            if (File.Exists(FilesystemConstants.LAUNCHER_VERSION_FILE))
            {
                Version localVersion = new(File.ReadAllText(FilesystemConstants.LAUNCHER_VERSION_FILE));

                if (onlineLauncherVersion.IsDifferentThan(localVersion))
                {
                    UpdateLauncher(onlineLauncherVersion);
                }
                else
                {
                    CheckForUpdates();
                }
            }
            else
            {
                UpdateLauncher(onlineLauncherVersion);
            }

            Dispatcher.Invoke(new Action(() => {
                VersionText.Text = new Version(File.ReadAllText(FilesystemConstants.LAUNCHER_VERSION_FILE)).ToString();
            }));
        }

        private void UpdateLauncher(Version _onlineLauncherVersion)
        {
            try
            {
                WebClient webClient = new WebClient();
                status = UpdaterState.DOWNLOADING_UPDATE;

                File.Move(FilesystemConstants.LAUNCHER_EXE, FilesystemConstants.TEMP_LAUNCHER_EXE);

                webClient.DownloadFileCompleted += new System.ComponentModel.AsyncCompletedEventHandler(LauncherDownloadCompletedCallback);
                webClient.DownloadFileAsync(new Uri(WebResourceConstants.LAUNCHER_DATA), FilesystemConstants.LAUNCHER_ZIP);
            }
            catch (Exception ex)
            {
                status = UpdaterState.FAILED;
                MessageBox.Show($"Error downloading game data: {ex}");
            }
        }
        private void LauncherDownloadCompletedCallback(object sender, AsyncCompletedEventArgs e)
        {
            try
            {
                string onlineVersion = ((Version)e.UserState).ToString();
                ZipFile.ExtractToDirectory(FilesystemConstants.LAUNCHER_ZIP, FilesystemConstants.ROOTPATH, overwriteFiles: true);
                File.Delete(FilesystemConstants.LAUNCHER_ZIP);

                File.WriteAllText(FilesystemConstants.LAUNCHER_VERSION_FILE, onlineVersion.ToString());

                status = UpdaterState.READY;

                ProcessStartInfo startInfo = new ProcessStartInfo(FilesystemConstants.LAUNCHER_EXE);
                Process.Start(startInfo);

                Window main = Application.Current.MainWindow;
                if (main != null)
                {
                    main.Close();
                }
            }
            catch (Exception ex)
            {
                status = UpdaterState.FAILED;
                MessageBox.Show($"Error installing game data: {ex}");
            }
        }

        private void CheckForUpdates()
        {
            if (File.Exists(FilesystemConstants.VERSION_FILE))
            {
                Version localVersion = new Version(File.ReadAllText(FilesystemConstants.VERSION_FILE));

                Dispatcher.BeginInvoke(new Action(() =>
                {
                    VersionText.Text = localVersion.ToString();
                }));

                try
                {
                    WebClient webClient = new();
                    Version onlineVersion = new(webClient.DownloadString(WebResourceConstants.GAME_VERSION_FILE));

                    if (onlineVersion.IsDifferentThan(localVersion))
                    {
                        InstallGameFiles(true, onlineVersion);
                    }
                    else
                    {
                        status = UpdaterState.READY;
                    }
                }
                catch (Exception ex)
                {
                    status = UpdaterState.FAILED;
                    MessageBox.Show($"Error checking for game updates: {ex}");
                }
            }
            else
            {
                InstallGameFiles(false, Version.zero);
            }
        }

        private void InstallGameFiles(bool _isUpdate, Version _onlineVersion)
        {
            try
            {
                WebClient webClient = new WebClient();
                if (_isUpdate)
                {
                    status = UpdaterState.DOWNLOADING_UPDATE;
                }
                else
                {
                    status = UpdaterState.DOWNLOADING_GAME;
                    _onlineVersion = new Version(webClient.DownloadString(WebResourceConstants.GAME_VERSION_FILE));
                }

                webClient.DownloadFileCompleted += new System.ComponentModel.AsyncCompletedEventHandler(DownloadCompletedCallback);
                webClient.DownloadFileAsync(new Uri(WebResourceConstants.GAME_DATA), FilesystemConstants.GAME_ZIP, _onlineVersion);
            }
            catch (Exception ex)
            {
                status = UpdaterState.FAILED;
                MessageBox.Show($"Error downloading game data: {ex}");
            }
        }

        private void DownloadCompletedCallback(object sender, AsyncCompletedEventArgs e)
        {
            try
            {
                string onlineVersion = ((Version)e.UserState).ToString();

                ZipFile.ExtractToDirectory(FilesystemConstants.GAME_ZIP, FilesystemConstants.ROOTPATH, overwriteFiles: true);
                File.Delete(FilesystemConstants.GAME_ZIP);

                File.WriteAllText(FilesystemConstants.VERSION_FILE, onlineVersion);

                status = UpdaterState.READY;
            }
            catch (Exception ex)
            {
                status = UpdaterState.FAILED;
                MessageBox.Show($"Error installing game data: {ex}");
            }
        }

        private void Window_ContentRendered(object sender, EventArgs e)
        {
            CheckForUpdates();
        }

        private void LoginButton_Click(object sender, RoutedEventArgs e)
        {
            if (status == UpdaterState.READY)
            {
                this.NavigationService.Navigate(new Uri("./Views/Dashboard.xaml", UriKind.Relative));
            }
            else if (status == UpdaterState.FAILED)
            {
                CheckForUpdates();
            }
        }
        private void RegisterButton_Click(object sender, RoutedEventArgs e)
        {
            if (status == UpdaterState.READY)
            {
                this.NavigationService.Navigate(new Uri("./Views/Register.xaml", UriKind.Relative));
            }
        }
    }

    struct Version
    {
        internal static Version zero = new(0, 0, 0);

        private readonly short major;
        private readonly short minor;
        private readonly short patch;

        internal Version(short _major, short _minor, short _patch)
        {
            major = _major;
            minor = _minor;
            patch = _patch;
        }

        internal Version(string _version)
        {
            string[] _versionStrings = _version.Split('.');
            if (_versionStrings.Length != 3)
            {
                major = 0;
                minor = 0;
                patch = 0;
                return;
            }

            major = short.Parse(_versionStrings[0]);
            minor = short.Parse(_versionStrings[1]);
            patch = short.Parse(_versionStrings[2]);
        }
        internal bool IsDifferentThan(Version _otherVersion)
        {
            return major != _otherVersion.major
                || minor != _otherVersion.minor
                || patch != _otherVersion.patch;
        }

        public override string ToString()
        {
            return $"{major}.{minor}.{patch}";
        }
    }
}
