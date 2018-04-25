using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;
using toDoList.Common;
using Windows.Data.Xml.Dom;
using Windows.System.Threading;
using Windows.UI.Notifications;

namespace toDoList.Services
{
    public class TileService
    {
        private static string path = "LiveTile.xml";
        //private static int index = 0;
        //public static ThreadPoolTimer threadPoolTimer;

        public static void CreateTile()
        {
            //原生NotificationQueue
            TileUpdateManager.CreateTileUpdaterForApplication().EnableNotificationQueue(true);

            /* 通过线程任务来手动更新。。 */
            //TileUpdateManager.CreateTileUpdaterForApplication().Clear();
            //UpdataTile();

            //添加定时任务
            //TimeSpan period = TimeSpan.FromSeconds(5);
            //threadPoolTimer = ThreadPoolTimer.CreatePeriodicTimer((source) =>
            //{
            //    UpdataTile();
            //},  period);
        }

        public static void UpdateTile(string title, string description)
        {
            //var viewModel = VM.Vm;
            //if (index >= viewModel.Items.Count)
            //    index = 0;
            //string title = viewModel.Items[index].Title;
            //string description = viewModel.Items[index].Description;
            //++index;

            XDocument xdoc = XDocument.Load(path);
            var xml = string.Format(xdoc.ToString(), title, description);
            var doc = new XmlDocument();

            doc.LoadXml(xml);
            var notification = new TileNotification(doc);
            TileUpdateManager.CreateTileUpdaterForApplication().Update(notification);
        }

        internal static void Reset()
        {
            var notifier = TileUpdateManager.CreateTileUpdaterForApplication();
            notifier.Clear();

            var Items = VM.Vm.Items;
            foreach (var item in Items)
            {
                UpdateTile(item.Title, item.Description);
            }
        }
    }
}
