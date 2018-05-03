using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Storage;
using Windows.Storage.FileProperties;
using Windows.Storage.Pickers;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media.Imaging;

namespace Muyer.Common
{
    class MediaHelper
    {
        private MediaElement player;
        private StorageFile file;
        private MusicProperties musicProperties;
        public bool MediaType { get; private set; }

        public MediaHelper(MediaElement me)
        {
            player = me;
        }

        public async  Task SetDefault()
        {
            file = await StorageFile.GetFileFromApplicationUriAsync(new Uri("ms-appx:///Assets/Animals.mp4"));
            await SetPlayer();
        }

        public async Task SetPlayer()
        {
            var stream = await file.OpenReadAsync();
            player.SetSource(stream, file.FileType);
            musicProperties = await file.Properties.GetMusicPropertiesAsync();
            MediaType = file.FileType != ".mp3";
        }

        public double GetDuration => musicProperties.Duration.TotalSeconds;

        public string GetArtist => musicProperties.Artist;

        public string GetTitle => musicProperties.Title;

        public async Task<bool> PickFile()
        {
            FileOpenPicker picker = new FileOpenPicker
            {
                ViewMode = PickerViewMode.Thumbnail,
                SuggestedStartLocation = PickerLocationId.Desktop
            };
            picker.FileTypeFilter.Add(".mp3");
            picker.FileTypeFilter.Add(".mp4");

            StorageFile storageFile = await picker.PickSingleFileAsync();

            if (storageFile != null)
            {
                file = storageFile;
                await SetPlayer();
                return true;
            }
            else
            {
                return false;
            }
        }

        public async Task<BitmapImage> GetCover()
        {
            using (StorageItemThumbnail thumbnail = await file.GetThumbnailAsync(ThumbnailMode.MusicView, 300))
            {
                if (thumbnail != null && thumbnail.Type == ThumbnailType.Image)
                {
                    var bitmapImage = new BitmapImage();
                    bitmapImage.SetSource(thumbnail);
                    return bitmapImage;
                }
                else
                {
                    return new BitmapImage {
                        UriSource = new Uri("ms-appx:///Assets/test.jpg")
                    };
                }
            }
        }

    }
}
