using toDoList.Common;
using toDoList.ViewModels;
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
        private ListItemViewModels Items = VM.vm;
        Frame Root => Window.Current.Content as Frame;
        public NewPage()
        {
            this.InitializeComponent();
            if (Items.SeletedItem != null)
                editor.SetInfo(Items.SeletedItem.Title, Items.SeletedItem.Description,
                    Items.SeletedItem.Duedate, Items.SeletedItem.Bitmap);
        }

        /// <summary>
        /// 点击底部Add按钮时清空信息
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Add_Click(object sender, RoutedEventArgs e)
        {
            Items.SeletedItem = null;
            editor.Clear();
        }

        /// <summary>
        /// 点击底部Delete按钮时触发
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Delete_Click(object sender, RoutedEventArgs e)
        {
            Items.Remove();
            if (Root.CanGoBack) Root.GoBack();
        }

        /// <summary>
        /// 离开时触发
        /// </summary>
        /// <param name="e"></param>
        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            if (Items.SeletedItem != null) Items.SeletedItem = null;
        }

    }
}


