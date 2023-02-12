using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.IO.Compression;
using System.Net;
using System.Windows;

namespace Updater
{
    enum UpdaterState
    {
        READY,
        FAILED,
        DOWNLOADING_GAME,
        DOWNLOADING_UPDATE
    }

    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private string rootpath;
        private string versionFile;
        private string gameZip;
        private string gameExe;

        private UpdaterState _status;
        internal UpdaterState status
        {
            get => _status;

            set
            {
                _status = value;
                switch(_status)
                {
                    case UpdaterState.READY:
                        PlayButton.Content = "Play";
                        break;
                    case UpdaterState.FAILED:
                        PlayButton.Content = "Update Failed";
                        break;
                    case UpdaterState.DOWNLOADING_GAME:
                        PlayButton.Content = "Downloading Game";
                        break;
                    case UpdaterState.DOWNLOADING_UPDATE:
                        PlayButton.Content = "Downloading Update";
                        break;
                    default:
                        break;
                }
            }
        }

        public MainWindow()
        {
            InitializeComponent();

            rootpath = Directory.GetCurrentDirectory();
            versionFile = Path.Combine(rootpath, "pmg_version");
            gameZip = Path.Combine(rootpath, "PMG.zip");
            gameExe = Path.Combine(rootpath, "Launcher.exe");
        }

        private void CheckForUpdates()
        {
            if(File.Exists(versionFile))
            {
                Version localVersion = new Version(File.ReadAllText(versionFile));
                VersionText.Text = localVersion.ToString();

                try
                {
                    WebClient webClient = new WebClient();
                    Version onlineVersion = new Version(webClient.DownloadString("https://drive.google.com/file/d/1xx3scGTQJ9gzM98bDSYlH8xY3KUn7dNz/view?usp=share_link"));

                    if(onlineVersion.IsDifferentThan(localVersion))
                    {
                        InstallGameFiles(true, onlineVersion);
                    }
                    else
                    {
                        status = UpdaterState.READY;
                    }
                }
                catch(Exception ex)
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
                    _onlineVersion = new Version(webClient.DownloadString("https://drive.google.com/uc?export=download&id=1xx3scGTQJ9gzM98bDSYlH8xY3KUn7dNz"));
                }

                webClient.DownloadFileCompleted += new System.ComponentModel.AsyncCompletedEventHandler(DownloadCompletedCallback);
                webClient.DownloadFileAsync(new Uri("https://drive.google.com/uc?export=download&id=1GQ5rVhmVgtE9BwkaEEODTOGImcs3X7bI"), gameZip, _onlineVersion);
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
                ZipFile.ExtractToDirectory(gameZip, rootpath, true);
                File.Delete(gameZip);

                File.WriteAllText(versionFile, onlineVersion);

                VersionText.Text = onlineVersion;
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
            if(File.Exists(gameExe) && status == UpdaterState.READY)
            {
                ProcessStartInfo startInfo = new ProcessStartInfo(gameExe);
                Process.Start(startInfo);

                Close();
            }
            else if(status == UpdaterState.FAILED)
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
