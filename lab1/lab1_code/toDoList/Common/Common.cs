using System;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Threading.Tasks;
using toDoList.ViewModels;
using Windows.Graphics.Imaging;
using Windows.Storage;
using Windows.UI.Xaml.Media.Imaging;

namespace toDoList.Common
{
    /// <summary>
    /// ViewModel和Model的基类
    /// </summary>
    public class NotifyBase: INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        public void NotifyPropertyChanged([CallerMemberName] String propertyName = "")
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }

    /// <summary>
    /// 静态ViewModel实例
    /// </summary>
    public class VM
    {
        static public ListItemViewModels vm = new ListItemViewModels();
    }

    /// <summary>
    /// 图片选取
    /// </summary>
    public class PictureHandler
    {
        static private bool IsOpened = false;
        static public async Task<BitmapImage> Picker()
        {
            if (IsOpened == true)
                return null;
            else
                IsOpened = true;
            BitmapImage bitmap = null;
            Windows.Storage.Pickers.FileOpenPicker picker 
                = new Windows.Storage.Pickers.FileOpenPicker
                {
                    ViewMode = Windows.Storage.Pickers.PickerViewMode.Thumbnail,
                    SuggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.PicturesLibrary
                };

            picker.FileTypeFilter.Clear();
            picker.FileTypeFilter.Add(".jpg");
            picker.FileTypeFilter.Add(".png");
            picker.FileTypeFilter.Add(".jpeg");

            StorageFile imgFile = await picker.PickSingleFileAsync();

            if (imgFile != null)
            {
                using (Windows.Storage.Streams.IRandomAccessStream fileStream
                    = await imgFile.OpenAsync(Windows.Storage.FileAccessMode.Read))
                {
                    bitmap = new BitmapImage();
                    bitmap.SetSource(fileStream);
                    IsOpened = false;
                }
            }
            else
            {
                IsOpened = false;
            }
            return bitmap;
        }
    }

}
