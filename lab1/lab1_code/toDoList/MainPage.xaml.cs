using toDoList.Common;
using toDoList.Models;
using toDoList.ViewModels;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;
// https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x804 上介绍了“空白页”项模板

namespace toDoList
{
    /// <summary>
    /// 可用于自身或导航至 Frame 内部的空白页。
    /// </summary>
    public sealed partial class MainPage : Page
    {
        private ListItemViewModels Items = VM.vm;
        Frame Root => Window.Current.Content as Frame;

        public MainPage()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// 点击底部Add按钮触发
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Add_Click(object sender, RoutedEventArgs e)
        {
            if (Window.Current.Bounds.Width <= 800)
            {
                Root.Navigate(typeof(NewPage));
            }
            else
            {
                Items.SeletedItem = null;
                itemEditor.Clear();
            }
        }

        /// <summary>
        /// 点击底部Delete按钮触发
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Delete_Click(object sender, RoutedEventArgs e)
        {
            Items.Remove();
            itemEditor.Clear();
        }

        /// <summary>
        /// 选中某个Item触发
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Tdl_ItemClick(object sender, ItemClickEventArgs e)
        {
            Items.SeletedItem = e.ClickedItem as ListItem;
            if (Window.Current.Bounds.Width <= 800)
                Root.Navigate(typeof(NewPage));
            else
                itemEditor.SetInfo(Items.SeletedItem.Title, Items.SeletedItem.Description, 
                    Items.SeletedItem.Duedate, Items.SeletedItem.Bitmap);
        }

        /// <summary>
        /// 更改背景图片
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void BgChange_Click(object sender, RoutedEventArgs e)
        {
            BitmapImage bgImg = await PictureHandler.Picker();
            if (bgImg == null) return;
            ImageBrush bg = (ImageBrush)Application.Current.Resources["bgImg"];
            bg.ImageSource = bgImg;
        }
    }

}
