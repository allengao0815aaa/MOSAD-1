using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Explore.Services;
using Windows.UI.Xaml.Media.Imaging;
using System.Text.RegularExpressions;

// https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x804 上介绍了“空白页”项模板

namespace Explore
{
    /// <summary>
    /// 可用于自身或导航至 Frame 内部的空白页。
    /// </summary>
    public sealed partial class MainPage : Page
    {
        private DateTimeOffset? Today => DateTimeOffset.Now;

        public MainPage()
        {
            this.InitializeComponent();
        }

        private async void HT_Button_Click(object sender, RoutedEventArgs e)
        {
            DateTimeOffset date = (DateTimeOffset)DatePicker.Date;
            NameValueCollection param = new NameValueCollection
            {
                ["yue"] = date.Month.ToString(),
                ["ri"] = date.Day.ToString(),
                ["rows"] = APIRequest.MAX_ROW.ToString(),
                ["type"] = country.IsChecked == true ? "1" : "2"
            };
            string res = await APIRequest.GetData(param, APIRequest.hiostoryToday, APIRequest.JSON);
            HT_Result.Text = res != string.Empty ? res : "出现错误，请重试";
        }

        private async void IP_Button_Click(object sender, RoutedEventArgs e)
        {
            if (ValidateIP(IP_Input.Text) != true) return;
            NameValueCollection param = new NameValueCollection
            {
                ["ip"] = IP_Input.Text,
                ["dtype"] = "XML"
            };
            string res = await APIRequest.GetData(param, APIRequest.IPSearch, APIRequest.XML);
            IP_Result.Text = res != string.Empty ? res : "出现错误，请重试";
        }

        private bool ValidateIP(string text)
        {
            string reg = @"^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$";
            Regex ip = new Regex(reg);
            return ip.IsMatch(text);
        }

        private void IP_Input_TextChanged(object sender, TextChangedEventArgs e)
        {
            TextBox tb = sender as TextBox;
            if (tb.Text == string.Empty) {
                status.Visibility = Visibility.Collapsed;
            }
            else if (ValidateIP(tb.Text))
            {
                status.Source = new BitmapImage(new Uri("ms-appx:///Assets/sign-check-icon.png"));
                status.Visibility = Visibility.Visible;
            }
            else
            {
                status.Source = new BitmapImage(new Uri("ms-appx:///Assets/sign-error-icon.png"));
                status.Visibility = Visibility.Visible;
            }
        }

        private void EnterDown(object sender, KeyRoutedEventArgs e)
        {
            if (e.Handled)
                return;
            StackPanel panel = sender as StackPanel;
            if (e.Key == Windows.System.VirtualKey.Enter)
            {
                e.Handled = true;
                if (panel == HistoryToday)
                    HT_Button_Click(HT_Button, new RoutedEventArgs());
                else
                    IP_Button_Click(IP_Button, new RoutedEventArgs());
            }
        }
    }

}
