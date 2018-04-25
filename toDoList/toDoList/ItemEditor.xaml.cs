using System;
using toDoList.Common;
using toDoList.Models;
using toDoList.ViewModels;
using Windows.Storage;
using Windows.Storage.AccessCache;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media.Imaging;

//https://go.microsoft.com/fwlink/?LinkId=234236 上介绍了“用户控件”项模板

namespace toDoList
{
    public sealed partial class ItemEditor : UserControl
    {
        private ListItem current;
        public ListItem Current { get => current; set => current = value; }

        private ListItemViewModels Items;

        public ItemEditor()
        {
            current = new ListItem(int.MinValue, "", "", DateTimeOffset.Now, null, null);
            Items = VM.Vm;
            this.InitializeComponent();
            this.DataContext = this;
        }

        #region Suspend And Resume
        public ApplicationDataCompositeValue OnSuspending()
        {
            ApplicationDataCompositeValue composite = new ApplicationDataCompositeValue
            {
                ["title"] = Current.Title,
                ["description"] = Current.Description,
                ["duedate"] = Current.Duedate,
            };

            return composite;
        }

        public void OnResuming(ApplicationDataCompositeValue composite, byte[] pixels = null)
        {
            Current.Title = (string)composite["title"];
            Current.Description = (string)composite["description"];
            Current.Duedate = (DateTimeOffset)composite["duedate"];
            if (pixels != null)
            {
                Current.Pixels = pixels;
            }
        }
        #endregion

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

        private async void CreateOrUpdate_Click(object sender, RoutedEventArgs e)
        {
            string errorMsg = Check(), msg = CreateOrUpdate.Content.ToString();
            ContentDialog dialog;
            if (errorMsg == "")
            {
                if (Items.SelectedItem == null)
                    Items.Add(current.Title, current.Description, current.Duedate, current.Pixels);
                else
                    Items.Update(current.Title, current.Description, current.Duedate, current.Pixels);
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

        public void Clear()
        {
            current.Title = "";
            current.Description = "";
            current.Duedate = DateTimeOffset.Now;
            current.Pixels = PictureHandler.DefaultPixels;

            if (ApplicationData.Current.LocalSettings.Values.ContainsKey("Current") &&
                StorageApplicationPermissions.FutureAccessList.ContainsItem(
                    (string)ApplicationData.Current.LocalSettings.Values["Current"]))
                StorageApplicationPermissions.FutureAccessList.Remove(
                    (string)ApplicationData.Current.LocalSettings.Values["Current"]);
        }

        private void Cancel_Click(object sender, RoutedEventArgs e)
        {
            Clear();
        }

        private async void Pictures_Click(object sender, RoutedEventArgs e)
        {
            byte[] pixels = await PictureHandler.Picker() ;
            if (pixels != null)
            {
                BitmapImage bitmap = await PictureHandler.AsBitmapImage(pixels);
                current.Pixels = pixels;
            }
        }

        public void SetInfo(string t, string d, DateTimeOffset dateTimeOffset, byte[] bytes)
        {
            current.Title = t;
            current.Description = d;
            current.Duedate = dateTimeOffset;
            current.Pixels = bytes;
        }

    }
}
