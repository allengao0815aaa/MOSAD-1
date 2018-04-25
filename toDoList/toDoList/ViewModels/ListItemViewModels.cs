using System;
using System.Collections.ObjectModel;
using System.Text;
using toDoList.Common;
using toDoList.Models;
using toDoList.Services;
using Windows.Storage;
using Windows.UI.Xaml.Media.Imaging;

namespace toDoList.ViewModels
{

    public class ListItemViewModels: NotifyBase
    {
        private ObservableCollection<ListItem> items = new ObservableCollection<ListItem>();
        public ObservableCollection<ListItem> Items
        {
            get => items;
        }

        private ListItem selectedItem = null;
        public ListItem SelectedItem
        {
            get => selectedItem;
            set
            {
                selectedItem = value;
                NotifyPropertyChanged();
            }
        }

        public static int count = 1;

        public void Add(string title, string description, DateTimeOffset dateTimeOffset, byte[] pixels = null)
        {
            ListItem res = new ListItem(count, title, description, dateTimeOffset, pixels);
            items.Add(res);

            TileService.UpdateTile(title, description);
            SQLiteService.Insert(count, title, description, dateTimeOffset, res.Pixels, false);
            ++count;
        }

        public void Remove()
        {
            if (SelectedItem != null)
            {
                SQLiteService.Delete(selectedItem.Id);
                items.Remove(SelectedItem);
            }
            SelectedItem = null;
            TileService.Reset();
        }

        public void Update(string title, string description, DateTimeOffset dateTimeOffset, byte[] bytes)
        {
            if (SelectedItem == null) return;
            SelectedItem.Title = title;
            SelectedItem.Description = description;
            SelectedItem.Duedate = dateTimeOffset;
            selectedItem.Pixels = bytes;


            TileService.Reset();
            SQLiteService.Update(selectedItem.Id, title, description, dateTimeOffset, bytes, selectedItem.Completed);
             SelectedItem = null;
        }

        public void SaveComplete()
        {
            foreach (ListItem temp in items)
            {
                SQLiteService.Update(temp.Id, temp.Title, temp.Description, temp.Duedate, temp.Pixels, temp.Completed);
            }
        }

        public string Query(string toQuery)
        {
            if (string.IsNullOrEmpty(toQuery)) return string.Empty;

            //Parse
            string[] queryList;
            bool AndOrOr = true; 
            if (toQuery.Contains("&"))
            {
                queryList = toQuery.Split('&');
            }
            else if (toQuery.Contains("|"))
            {
                queryList = toQuery.Split('|');
                AndOrOr = false;
            }
            else
            {
                queryList = toQuery.Split(toQuery.ToCharArray());
            }

            //Query
            StringBuilder res = new StringBuilder();
            foreach (ListItem item in items)
            {
                string temp = item.Title + " " + item.Description + " " + item.Duedate.ToString("d") + "\n";
                bool success = AndOrOr;
                foreach (string queryString in queryList)
                {
                    if (temp.Contains(queryString))
                    {       
                        success = true;
                        if (AndOrOr == false)
                            break;
                    }
                    else if (AndOrOr)
                    {
                        success = false;
                        break;
                    }
                }
                if (success)
                    res.Append(temp);
            }
            return res.ToString();
        }
    }
}
