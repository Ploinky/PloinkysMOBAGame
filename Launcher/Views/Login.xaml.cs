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
                switch (_status)
                {
                    case UpdaterState.READY:
                        PlayButton.Content = "Play";
                        PlayButton.IsEnabled = true;
                        break;
                    case UpdaterState.FAILED:
                        PlayButton.Content = "Update Failed, Retry";
                        PlayButton.IsEnabled = true;
                        break;
                    case UpdaterState.DOWNLOADING_GAME:
                        PlayButton.Content = "Downloading Game";
                        PlayButton.IsEnabled = false;
                        break;
                    case UpdaterState.DOWNLOADING_UPDATE:
                        PlayButton.Content = "Downloading Update";
                        PlayButton.IsEnabled = false;
                        break;
                    default:
                        break;
                }
            }
        }

        public Login()
        {
            InitializeComponent();
            CheckForLauncherUpdates();
        }

        private void CheckForLauncherUpdates()
        {
            var fi = new System.IO.FileInfo(FilesystemConstants.TEMP_LAUNCHER_EXE);
            if (fi.Exists)
            {
                fi.Delete();
                fi.Refresh();
                while (fi.Exists)
                {
                    System.Threading.Thread.Sleep(100);
                    fi.Refresh();
                }
            }

            WebClient webClient = new WebClient();
            Version onlineLauncherVersion = new Version(webClient.DownloadString(WebResourceConstants.LAUNCHER_VERSION_FILE));

            if (File.Exists(FilesystemConstants.LAUNCHER_VERSION_FILE))
            {
                Version localVersion = new Version(File.ReadAllText(FilesystemConstants.LAUNCHER_VERSION_FILE));

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


            VersionText.Text = new Version(File.ReadAllText(FilesystemConstants.LAUNCHER_VERSION_FILE)).ToString();
        }

        private void UpdateLauncher(Version _onlineLauncherVersion)
        {
            try
            {
                WebClient webClient = new WebClient();
                status = UpdaterState.DOWNLOADING_UPDATE;

                File.Move(FilesystemConstants.LAUNCHER_EXE, FilesystemConstants.TEMP_LAUNCHER_EXE);

                webClient.DownloadFile(new Uri(WebResourceConstants.LAUNCHER_DATA), FilesystemConstants.LAUNCHER_ZIP);

                ZipFile.ExtractToDirectory(FilesystemConstants.LAUNCHER_ZIP, FilesystemConstants.ROOTPATH, overwriteFiles: true);
                File.Delete(FilesystemConstants.LAUNCHER_ZIP);

                File.WriteAllText(FilesystemConstants.LAUNCHER_VERSION_FILE, _onlineLauncherVersion.ToString());

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
                MessageBox.Show($"Error downloading game data: {ex}");
            }
        }

        private void CheckForUpdates()
        {
            if (File.Exists(FilesystemConstants.VERSION_FILE))
            {
                Version localVersion = new Version(File.ReadAllText(FilesystemConstants.VERSION_FILE));
                VersionText.Text = localVersion.ToString();

                try
                {
                    WebClient webClient = new WebClient();
                    Version onlineVersion = new Version(webClient.DownloadString(WebResourceConstants.GAME_VERSION_FILE));

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

        private void PlayButton_Click(object sender, RoutedEventArgs e)
        {
            if (File.Exists(FilesystemConstants.GAME_EXE) && status == UpdaterState.READY)
            {
                this.NavigationService.Navigate(new Uri("./Views/Dashboard.xaml", UriKind.Relative));
            }
            else if (status == UpdaterState.FAILED)
            {
                CheckForUpdates();
            }
        }
    }

    struct Version
    {
        internal static Version zero = new Version(0, 0, 0);

        private short major;
        private short minor;
        private short patch;

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
