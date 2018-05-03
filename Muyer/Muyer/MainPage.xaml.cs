using Muyer.Common;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Media.Core;
using Windows.Media.Playback;
using Windows.System;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x804 上介绍了“空白页”项模板

namespace Muyer
{
    /// <summary>
    /// 可用于自身或导航至 Frame 内部的空白页。
    /// </summary>
    public sealed partial class MainPage : Page
    {
        DispatcherTimer timer;
        MediaHelper helper;
        int width = 800;
        int height = 600;

        public MainPage()
        {
            this.InitializeComponent();

            timer = new DispatcherTimer();
            timer.Tick += SliderValueChange;
            timer.Interval = new TimeSpan(0, 0, 0, 1);

            InitHelper();

            Window.Current.CoreWindow.KeyUp += SpaceShortCut;
        }

        private async void InitHelper()
        {
            helper = new MediaHelper(player);
            await helper.SetDefault();
            await SetInfo();
        }

        private async  Task SetInfo()
        {
            cover.Fill = new ImageBrush
            {
                ImageSource = await helper.GetCover()
            };

            progressBar.Maximum = helper.GetDuration;
            endAt.Text = TimeSpan.FromSeconds(progressBar.Maximum).ToString(@"mm\:ss");
            MediaInfo.Text = string.Format("Playing - {0}   {1}", helper.GetTitle, helper.GetArtist);

            if (helper.MediaType)
            {
                cover.Visibility = Visibility.Collapsed;
                player.Width = width;
                player.Height = height;
            }
            else
            {
                cover.Visibility = Visibility.Visible;
            }
        }

        private void PlayEnds(object sender, RoutedEventArgs e)
        {
            timer.Stop();
            ResetProcess();
        }

        private void SliderValueChange(object sender, object e)
        {
            progressBar.ValueChanged -= SeekPosition;
            progressBar.Value = player.Position.TotalSeconds;
            progressBar.ValueChanged += SeekPosition;
            currentAt.Text = player.Position.ToString(@"mm\:ss");
        }

        private void PlayButton_Click(object sender, RoutedEventArgs e)
        {
            if (player.CurrentState == MediaElementState.Playing)
            {
                player.Pause();
                rotateBoard.Pause();
                timer.Stop();
                playButton.Icon = new SymbolIcon(Symbol.Play);
            }
            else
            {
                player.Play();
                rotateBoard.Begin();
                timer.Start();
                playButton.Icon = new SymbolIcon(Symbol.Pause);
            }
        }

        private void PauseButton_Click(object sender, RoutedEventArgs e)
        {
            player.Pause();
            rotateBoard.Pause();
            timer.Stop();
        }

        private void StopButton_Click(object sender, RoutedEventArgs e)
        {
            player.Stop();
            ResetProcess();
            timer.Stop();
        }

        private void SeekPosition(object sender, RangeBaseValueChangedEventArgs e)
        {
            var diff = e.NewValue - e.OldValue;
            player.Position = TimeSpan.FromSeconds(((Slider)sender).Value);
            currentAt.Text = player.Position.ToString(@"mm\:ss");
        }

        private void FullScreenButton_Click(object sender, RoutedEventArgs e)
        {
            if (!helper.MediaType)
            {
                return;
            }
            if (Double.IsNaN(player.Width))
            {
                player.Width = width;
                player.Height = height;
            }
            else
            {
                player.Width = Double.NaN;
                player.Height = Double.NaN;
            }
        }

        private void ResetProcess()
        {
            progressBar.Value = 0;
            currentAt.Text = "00:00";
            playButton.Icon = new SymbolIcon(Symbol.Play);
            rotateBoard.Stop();
        }

        private async void OpenFileButton_Click(object sender, RoutedEventArgs e)
        {
            bool flag = await helper.PickFile();
            if (flag)
            {
                await SetInfo();
                ResetProcess();
            }
        }

        private void SpaceShortCut(CoreWindow sender, KeyEventArgs e)
        {
            e.Handled = true;
            if (e.VirtualKey == VirtualKey.Space)
            {
                PlayButton_Click(null, new RoutedEventArgs());
            }
        }

        private void Player_PointerPressed(object sender, PointerRoutedEventArgs e)
        {
            PlayButton_Click(null, new RoutedEventArgs());
        }

    }
}
