using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Storage;
using Windows.Storage.AccessCache;
using Windows.Storage.Streams;
using Windows.UI.Xaml.Media.Imaging;

namespace toDoList.Common
{
    public class PictureHandler
    {
        private static bool IsOpened = false;

        public static string DefaultImageUri
        {
            get;
        } = "ms-appx:///Assets/timg.jpg";

        public static byte[] DefaultPixels { get; private set; }

        public static async Task SetDefaultPixels()
        {
            StorageFile imgFile;
            imgFile = await StorageFile.GetFileFromApplicationUriAsync(new Uri(DefaultImageUri));
            DefaultPixels = await AsByteArray(imgFile);
        }

        public static async Task<byte[]> Picker()
        {
            if (IsOpened == true)
                return null;
            else
                IsOpened = true;

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
            byte[] pixels = null;
            if (imgFile != null)
            {
                pixels = await AsByteArray(imgFile);
            }
            IsOpened = false;
            return pixels;
        }

        public static async Task<byte[]> AsByteArray(StorageFile file)
        {
            var stream = await file.OpenStreamForReadAsync();
            var bytes = new byte[(int)stream.Length];
            stream.Read(bytes, 0, (int)stream.Length);
            return bytes;
        }

        public static async Task<BitmapImage> AsBitmapImage(byte[] pixels)
        {
            if (pixels == null) return null;
            BitmapImage bitmap = new BitmapImage();
            using (MemoryStream stream = new MemoryStream(pixels))
            {
                await bitmap.SetSourceAsync(stream.AsRandomAccessStream());
            }
            return bitmap;
        }

        public static async Task<StorageFile> AsStorageFile(byte[] pixels, string name, bool SettingOrTemp = false)
        {
            StorageFolder tempFolder = SettingOrTemp ? ApplicationData.Current.LocalFolder : ApplicationData.Current.LocalCacheFolder;
            StorageFile img = await tempFolder.CreateFileAsync(name, CreationCollisionOption.ReplaceExisting);
            await FileIO.WriteBytesAsync(img, pixels);
            return img;
        }
    }
}
