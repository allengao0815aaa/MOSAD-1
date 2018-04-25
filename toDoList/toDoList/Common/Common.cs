using System;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using toDoList.Services;
using toDoList.ViewModels;
using Windows.ApplicationModel;
using Windows.Graphics.Display;
using Windows.Graphics.Imaging;
using Windows.Storage;
using Windows.Storage.AccessCache;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;

namespace toDoList.Common
{

    public class NotifyBase: INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        public void NotifyPropertyChanged([CallerMemberName] String propertyName = "")
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }

    public class VM
    {
        private static ListItemViewModels vm;
        public static ListItemViewModels Vm { get => vm; set => vm = value; }

        private VM() { }

        public static void Init()
        {
            vm = new ListItemViewModels();
            SQLiteService.Load();
        }

    }

    public class CustomSetting
    {
        static string BgFile = "CustomBackground.jpg";
        static string key = "bgImg";
        static public async void ChangeBg(object sender, RoutedEventArgs e)
        {
            byte[] pixels = await PictureHandler.Picker();
            BitmapImage bgImg = await PictureHandler.AsBitmapImage(pixels);
            if (bgImg == null) return;
            ImageBrush bg = (ImageBrush)App.Current.Resources[key];
            bg.ImageSource = bgImg;

            await PictureHandler.AsStorageFile(pixels, BgFile, true);
        }

        static public async void LoadBg()
        {
            StorageFolder folder = ApplicationData.Current.LocalFolder;
            StorageFile file = await folder.TryGetItemAsync(BgFile) as StorageFile;
            if (file == null)
                return;
            byte[] pixels = await PictureHandler.AsByteArray(file);
            using (InMemoryRandomAccessStream ms = new InMemoryRandomAccessStream())
            {
                using (DataWriter writer = new DataWriter(ms.GetOutputStreamAt(0)))
                {
                    writer.WriteBytes(pixels);
                    writer.StoreAsync().GetResults();
                }
                var image = new BitmapImage();
                image.SetSource(ms);
                ImageBrush bg = (ImageBrush)App.Current.Resources[key];
                bg.ImageSource = image;
            }
        }
    }

}
