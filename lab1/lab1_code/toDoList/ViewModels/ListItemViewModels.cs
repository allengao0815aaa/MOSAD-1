using System;
using System.Collections.ObjectModel;
using toDoList.Common;
using toDoList.Models;
using Windows.UI.Xaml.Media.Imaging;

namespace toDoList.ViewModels
{
    public class ListItemViewModels: NotifyBase
    {
        private ObservableCollection<ListItem> items = new ObservableCollection<ListItem>();
        public ObservableCollection<ListItem> Items
        {
            get => items;
            set
            {
                items = value;
                NotifyPropertyChanged();
            }
        }

        private ListItem seletedItem = null;
        public ListItem SeletedItem
        {
            get => seletedItem;
            set
            {
                seletedItem = value;
                NotifyPropertyChanged();
            }
        }

        public ListItemViewModels()
        {
            items.Add(new ListItem("silly b", "1024", DateTimeOffset.Now));
            items.Add(new ListItem("silly a", "1024", DateTimeOffset.Now));
        }

        public void Add(string title, string description, DateTimeOffset dateTimeOffset, BitmapImage bitmap)
        {
            items.Add(new ListItem(title, description, dateTimeOffset, bitmap));
        }

        public void Remove()
        {
            if (SeletedItem != null)
                items.Remove(SeletedItem);
            SeletedItem = null;
        }

        public void Update(string title, string description, DateTimeOffset dateTimeOffset, BitmapImage bitmap)
        {
            if (SeletedItem == null) return;
            SeletedItem.Title = title;
            SeletedItem.Description = description;
            SeletedItem.Duedate = dateTimeOffset;
            SeletedItem.Bitmap = bitmap;
            SeletedItem = null;
        }
    }
}
