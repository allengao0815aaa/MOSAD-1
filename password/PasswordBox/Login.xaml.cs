﻿using System;
using System.Collections.Generic;
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
using PasswordBox;
using Windows.UI.Xaml.Media.Imaging;
using Windows.Storage;
using PasswordBox.ViewModel;
using PasswordBox.Model;
using PasswordBox.Common;
using PasswordBox.Services;

// https://go.microsoft.com/fwlink/?LinkId=234238 上介绍了“空白页”项模板

namespace PasswordBox
{
    /// <summary>
    /// 可用于自身或导航至 Frame 内部的空白页。
    /// </summary>
    public sealed partial class Login : Page
    {
        public Login()
        {
            this.InitializeComponent();
        }

        PersonalInfo Info = new PersonalInfo();

        /// <summary>
        /// 检验密码是否正确
        /// 若正确则跳转到home
        /// 否则提示密码错误
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void CheckPassword(object sender, RoutedEventArgs e)
        {
            // check the password success
            if (Crypto.TestEqual(Info.Password, checkPassword.Password))
            {
                App.loginFlag = true;
                Frame.Navigate(typeof(MainPage));
                //MainPage.Current.ShowMenu();
            }
            // check the password fail
            else
            {
                // caution the password error
                ContentDialog dialog;
                dialog = new ContentDialog()
                {
                    Title = "提示",
                    PrimaryButtonText = "确认",
                    Content = "密码错误",
                    FullSizeDesired = false,
                };
                dialog.PrimaryButtonClick += (_s, _e) => { };
                await dialog.ShowAsync();
            }
        }

        /// <summary>
        /// 忘记密码，跳转到更改密码页面
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ForgetPassword(object sender, RoutedEventArgs e)
        {
            Frame.Navigate(typeof(ChangePassword));
        }

        /// <summary>
        /// 按enter键登录
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void CheckPassword_KeyDown(object sender, KeyRoutedEventArgs e)
        {
            if (e.Key == Windows.System.VirtualKey.Enter)
            {
                CheckPassword(null, null);
            }
        }

        /// <summary>
        /// 进入登录页面时检查生日
        /// 彩蛋
        /// </summary>
        /// <param name="e"></param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            if (UserInfo.GetInfo("Birth") != null && CheckBirth())
            {
                bg.ImageSource = new BitmapImage { UriSource = new Uri("ms-appx:///Assets/birth.png") };
            }
            else
            {
                bg.ImageSource = new BitmapImage { UriSource = new Uri("ms-appx:///Assets/bg.png") };
            }
        }

        /// <summary>
        /// 检查今天是否为用户生日
        /// </summary>
        private bool CheckBirth()
        {
            return DateTimeOffset.Now.Month == Convert.ToDateTime(UserInfo.GetInfo("Birth")).Date.Month &&
                DateTimeOffset.Now.Day == Convert.ToDateTime(UserInfo.GetInfo("Birth")).Date.Day;
        }
    }
}
