using System;
using toDoList.Common;
using Windows.UI.Xaml.Media.Imaging;

namespace toDoList.Models
{
    public class ListItem: NotifyBase
    {
        private string id;
        private string title;
        private string description;
        private DateTimeOffset duedate;
        private BitmapImage bitmap;
        private bool? completed;

        public string Title
        {
            get => title;
            set
            {
                if (title != value)
                {
                    title = value;
                    NotifyPropertyChanged();
                }
            }
        }

        public string Description
        {
            get => description;
            set
            {
                if (description != value)
                {
                    description = value;
                    NotifyPropertyChanged();
                }
            }
        }
        public DateTimeOffset Duedate
        {
            get => duedate;
            set
            {
                if (duedate != value)
                {
                    duedate = value;
                    NotifyPropertyChanged();
                }
            }
        }
        public BitmapImage Bitmap
        {
            get => bitmap;
            set
            {
                if (bitmap != value)
                {
                    bitmap = value;
                    NotifyPropertyChanged();
                }
            }
        }
        public bool? Completed
        {
            get => completed;
            set
            {
                if (completed != value)
                {
                    completed = value;
                    NotifyPropertyChanged();
                }
            }
        }

        public ListItem(string t, string d, DateTimeOffset date, BitmapImage bi= null)
        {
            id = Guid.NewGuid().ToString();
            title = t;
            description = d;
            duedate = date;
            bitmap = bi ?? new BitmapImage
            {
                UriSource = new Uri("ms-appx:///Assets/timg.jpg")
            };
            completed = false;
        }

    }
}
