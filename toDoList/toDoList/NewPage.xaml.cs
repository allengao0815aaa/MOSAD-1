using toDoList.Common;
using toDoList.Models;
using toDoList.ViewModels;
using Windows.Storage;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
// https://go.microsoft.com/fwlink/?LinkId=234238 上介绍了“空白页”项模板

namespace toDoList
{
    /// <summary>
    /// 可用于自身或导航至 Frame 内部的空白页。
    /// </summary>
    public sealed partial class NewPage : Page
    {
        private ListItemViewModels Items;
        Frame Root => Window.Current.Content as Frame;

        public NewPage()
        {
            Items = VM.Vm;
            this.InitializeComponent();
            changeBg.Click += CustomSetting.ChangeBg;
            if (Items.SelectedItem != null)
            {
                ListItem temp = Items.SelectedItem;
                editor.SetInfo(temp.Title, temp.Description,
                     temp.Duedate, temp.Pixels);
            }
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            base.OnNavigatedTo(e);
            if (e.NavigationMode == NavigationMode.New)
            {
                ApplicationData.Current.LocalSettings.Values.Remove("NewPage");
            }
            else
            {
                if (ApplicationData.Current.LocalSettings.Values.ContainsKey("NewPage"))
                {
                    SuspendingManager manager = new SuspendingManager("NewPage");
                    manager.Resume(editor);
                }
            }
        }

        protected override async void OnNavigatedFrom(NavigationEventArgs e)
        {
            base.OnNavigatedFrom(e);
            if (((App)App.Current).isSuspending)
            {
                SuspendingManager manager = new SuspendingManager("NewPage");
                await manager.Suspend(editor);
            }
            else
            {
                if (Items.SelectedItem != null) Items.SelectedItem = null;
            }
        }

        private void Add_Click(object sender, RoutedEventArgs e)
        {
            Items.SelectedItem = null;
            editor.Clear();
        }

        private void Delete_Click(object sender, RoutedEventArgs e)
        {
            Items.Remove();
            if (Root.CanGoBack) Root.GoBack();
        }

    }
}


