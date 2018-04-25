using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using toDoList.Services;
using Windows.Storage;
using Windows.Storage.AccessCache;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;

namespace toDoList.Common
{
    public class SuspendingManager
    {
        string pageName;
        string current = "Current";
        string selected = "Selected";
        public SuspendingManager(string name)
        {
            pageName = name;
        }

        private bool ExistsImgFile(string name) =>
            ApplicationData.Current.LocalSettings.Values.ContainsKey(name) &&
                    StorageApplicationPermissions.FutureAccessList.ContainsItem(
                    (string)ApplicationData.Current.LocalSettings.Values[name]);

        public async void Resume(ItemEditor itemEditor)
        {
            var Items = VM.Vm;

            var composite = ApplicationData.Current.LocalSettings.Values[pageName]
                as ApplicationDataCompositeValue;
            if (ExistsImgFile(current))
            {
                StorageFile file = await StorageApplicationPermissions.FutureAccessList.GetFileAsync(
                    (string)ApplicationData.Current.LocalSettings.Values[current]);
                byte[] pixels = await PictureHandler.AsByteArray(file);

                itemEditor.OnResuming(composite, pixels);
            }
            else
            {
                itemEditor.OnResuming(composite);
            }
            ApplicationData.Current.LocalSettings.Values.Remove(pageName);

            if (ApplicationData.Current.LocalSettings.Values.ContainsKey(selected))
            {
                Items.SelectedItem = Items.Items[(int)ApplicationData.Current.LocalSettings.Values[selected]];
                ApplicationData.Current.LocalSettings.Values.Remove(selected);
            }
        }

        public async Task Suspend(ItemEditor itemEditor)
        {
            var Items = VM.Vm;

            var composite = itemEditor.OnSuspending();
            ApplicationData.Current.LocalSettings.Values[pageName] = composite;

            if (Items.SelectedItem != null)
                ApplicationData.Current.LocalSettings.Values[selected] = Items.Items.IndexOf(Items.SelectedItem);

            byte[] pixels = itemEditor.Current.Pixels;
            StorageFile imgFile = await PictureHandler.AsStorageFile(pixels, "Current.jpg");
            if (ExistsImgFile(current))
            {
                StorageApplicationPermissions.FutureAccessList.Remove((string)ApplicationData.Current.LocalSettings.Values[current]);
            }
            ApplicationData.Current.LocalSettings.Values[current] = StorageApplicationPermissions.FutureAccessList.Add(imgFile);
            //TileService.threadPoolTimer.Cancel();
        }
    }
}
