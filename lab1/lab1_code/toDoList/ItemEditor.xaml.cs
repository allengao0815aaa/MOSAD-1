using System;
using toDoList.Common;
using toDoList.Models;
using toDoList.ViewModels;
using Windows.Storage;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media.Imaging;

//https://go.microsoft.com/fwlink/?LinkId=234236 上介绍了“用户控件”项模板

namespace toDoList
{
    public sealed partial class ItemEditor : UserControl
    {
        public ItemEditor()
        {
            current = new ListItem("", "", DateTimeOffset.Now);
            this.InitializeComponent();
            this.DataContext = this;
        }

        private ListItem current;
        public ListItem Current { get => current; set => current = value; }

        private ListItemViewModels Items = VM.vm;

        /// <summary>
        /// 检查
        /// </summary>
        /// <returns>错误信息</returns>
        private string Check()
        {
            string errorMsg = string.Empty;
            if (current.Title == string.Empty)
                errorMsg += "Title不能为空\n";
            if (current.Description == string.Empty)
                errorMsg += "Description不能为空\n";
            if (DateTime.Compare(datePicker.Date.DateTime, DateTime.Today) < 0)
                errorMsg += "DueDate已过去\n";
            return errorMsg;
        }

        /// <summary>
        /// 点击CreateOrUpdate按钮触发
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void CreateOrUpdate_Click(object sender, RoutedEventArgs e)
        {
            string errorMsg = Check(), msg = CreateOrUpdate.Content.ToString();
            ContentDialog dialog;
            if (errorMsg == "")
            {
                if (Items.SeletedItem == null)
                    Items.Add(current.Title, current.Description, current.Duedate, current.Bitmap);
                else
                    Items.Update(current.Title, current.Description, current.Duedate, current.Bitmap);
                Clear();
                dialog = new ContentDialog()
                {
                    Title = "Done",
                    Content = msg + " succeeds!",
                    PrimaryButtonText = "OK",
                    FullSizeDesired = false,
                };
                dialog.PrimaryButtonClick += (_s, _e) => 
                {
                    if (Window.Current.Content is Frame root && root.CanGoBack)
                        root.GoBack();
                };
            }
            else
            {
                dialog = new ContentDialog()
                {
                    Title = "Error",
                    Content = errorMsg,
                    PrimaryButtonText = "OK",
                    FullSizeDesired = false,
                };
                dialog.PrimaryButtonClick += (_s, _e) => { };
            }
            await dialog.ShowAsync();
        }

        /// <summary>
        /// 清空
        /// </summary>
        public void Clear()
        {
            current.Title = "";
            current.Description = "";
            current.Duedate = DateTimeOffset.Now;
            current.Bitmap = new BitmapImage
            {
                UriSource = new Uri("ms-appx:///Assets/timg.jpg")
            };
        }

        /// <summary>
        /// 点击Cancel按钮触发
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Cancel_Click(object sender, RoutedEventArgs e)
        {
            Clear();
        }

        /// <summary>
        /// 点击Pictures按钮触发选取图片
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void Pictures_Click(object sender, RoutedEventArgs e)
        {
            BitmapImage bitmap = await PictureHandler.Picker();
            if (bitmap != null)
                current.Bitmap = bitmap;
        }

        /// <summary>
        /// 当选中某个Item时更新细节信息
        /// </summary>
        /// <param name="t"></param>
        /// <param name="d"></param>
        /// <param name="dateTimeOffset"></param>
        /// <param name="bitmap"></param>
        public void SetInfo(string t, string d, DateTimeOffset dateTimeOffset, BitmapImage bitmap)
        {
            current.Title = t;
            current.Description = d;
            current.Duedate = dateTimeOffset;
            current.Bitmap = bitmap;
        }

    }
}
