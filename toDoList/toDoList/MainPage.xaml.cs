using System;
using System.Collections.Generic;
using toDoList.Common;
using toDoList.Models;
using toDoList.Services;
using toDoList.ViewModels;
using Windows.ApplicationModel.DataTransfer;
using Windows.Foundation;
using Windows.Storage;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Navigation;
// https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x804 上介绍了“空白页”项模板

namespace toDoList
{
    /// <summary>
    /// 可用于自身或导航至 Frame 内部的空白页。
    /// </summary>
    public sealed partial class MainPage : Page
    {
        private ListItemViewModels Items;
        Frame Root => Window.Current.Content as Frame;

        public MainPage()
        {
            Items = VM.Vm;
            this.InitializeComponent();
            changeBg.Click += CustomSetting.ChangeBg;
        }

        #region Navigation
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            base.OnNavigatedTo(e);
            if (e.NavigationMode == NavigationMode.New)
            {
                ApplicationData.Current.LocalSettings.Values.Remove("MainPage");
            }
            else
            {
                if (ApplicationData.Current.LocalSettings.Values.ContainsKey("MainPage"))
                {
                    SuspendingManager manager = new SuspendingManager("MainPage");
                    manager.Resume(itemEditor);
                }
            }

            //App to App
            DataTransferManager dataTransferManager = DataTransferManager.GetForCurrentView();
            dataTransferManager.DataRequested += 
                new TypedEventHandler<DataTransferManager, DataRequestedEventArgs>(DataTransferManager_DataRequested);
        }

        protected override async void OnNavigatedFrom(NavigationEventArgs e)
        {
            base.OnNavigatedFrom(e);
            if (((App)App.Current).isSuspending)
            {
                SuspendingManager manager = new SuspendingManager("MainPage");
                await manager.Suspend(itemEditor);
            }

            //App to App
            DataTransferManager dataTransferManager = DataTransferManager.GetForCurrentView();
            dataTransferManager.DataRequested -= 
                new TypedEventHandler<DataTransferManager, DataRequestedEventArgs>(DataTransferManager_DataRequested);
        }
        #endregion

        #region App to App Communication
        private void DataTransferManager_DataRequested(DataTransferManager sender, DataRequestedEventArgs args)
        {
            DataRequest request = args.Request;
            GetShareContent(request);
        }

        private void MenuShare_Click(object sender, RoutedEventArgs e)
        {
            DataTransferManager.ShowShareUI();
        }

        private void Share_Click(object sender, RoutedEventArgs e)
        {
            if (Items.SelectedItem == null)
                return;
            DataTransferManager.ShowShareUI();
        }

        private void GetShareContent(DataRequest request)
        {
            DataPackage dp = request.Data;
            dp.Properties.Title = Items.SelectedItem.Title;
            dp.Properties.Description = Items.SelectedItem.Description;
            dp.SetText(Items.SelectedItem.Description);
            dp.SetDataProvider(StandardDataFormats.Bitmap,
                providerRequest => this.OnDeferredImageRequestedHandler(providerRequest, dp));
        }

        private async void OnDeferredImageRequestedHandler(DataProviderRequest request, DataPackage dp)
        {
            DataProviderDeferral deferral = request.GetDeferral();
            try
            {
                StorageFile img = await PictureHandler.AsStorageFile(Items.SelectedItem.Pixels, "ToShare.jpg");

                if (img != null)
                {
                    List<IStorageFile> imageItems = new List<IStorageFile>
                    {
                        img
                    };
                    dp.SetStorageItems(imageItems);

                    RandomAccessStreamReference imageStreamRef = RandomAccessStreamReference.CreateFromFile(img);
                    dp.Properties.Thumbnail = imageStreamRef;
                    dp.SetBitmap(imageStreamRef);
                }
            }
            catch (Exception e)
            {
                throw e;
            }
            deferral.Complete();
        }
        #endregion

        #region Functional Button
        private void Add_Click(object sender, RoutedEventArgs e)
        {
            if (Window.Current.Bounds.Width <= 800)
            {
                Root.Navigate(typeof(NewPage));
            }
            else
            {
                Items.SelectedItem = null;
                itemEditor.Clear();
            }
        }

        private void Delete_Click(object sender, RoutedEventArgs e)
        {
            Delete();
        }

        private void MenuDelete_Click(object sender, RoutedEventArgs e)
        {
            Delete();
        }

        private void Delete()
        {
            Items.Remove();
            itemEditor.Clear();
        }

        private void MenuEdit_Click(object sender, RoutedEventArgs e)
        {
            Edit();
        }

        private void Tdl_ItemClick(object sender, ItemClickEventArgs e)
        {
            Items.SelectedItem = e.ClickedItem as ListItem;
            Edit();
        }

        private void Edit()
        {
            if (Window.Current.Bounds.Width <= 800)
                Root.Navigate(typeof(NewPage));
            else
            {
                ListItem temp = Items.SelectedItem;
                itemEditor.SetInfo(temp.Title, temp.Description,
                     temp.Duedate, temp.Pixels);
            }
        }

        private void Setting_Click(object sender, RoutedEventArgs e)
        {
            Items.SelectedItem = (sender as FrameworkElement).DataContext as ListItem;
        }
        #endregion

        #region Search
        private async void Search_Click(object sender, RoutedEventArgs e)
        {
            if (string.IsNullOrEmpty(queryString.Text)) return;
            string toQuery = queryString.Text;
            string res = Items.Query(toQuery);
            if (string.IsNullOrEmpty(res)) res = "Not found";
            var dialog = new ContentDialog()
            {
                Title = "Result",
                Content = res,
                PrimaryButtonText = "OK",
                FullSizeDesired = false,
            };
            dialog.PrimaryButtonClick += (_s, _e) => { };
            await dialog.ShowAsync();
        }

        private void QueryString_KeyDown(object sender, Windows.UI.Xaml.Input.KeyRoutedEventArgs e)
        {
            if (e.Key == Windows.System.VirtualKey.Enter)
            {
                Search_Click(null, null);
                e.Handled = true;
            }
        }
        #endregion
    }

}
