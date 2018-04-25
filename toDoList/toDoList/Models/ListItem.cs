using System;
using toDoList.Common;
using toDoList.Services;
using Windows.Storage;
using Windows.UI.Xaml.Media.Imaging;

namespace toDoList.Models
{
    public class ListItem: NotifyBase
    {
        private int id;
        private string title;
        private string description;
        private DateTimeOffset duedate;
        private byte[] pixels;
        private bool? completed = false;

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
        public byte[] Pixels
        {
            get => pixels;
            set
            {
                if (pixels != value)
                {
                    pixels = value;
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
                    SQLiteService.SetComplete(id, completed);
                    NotifyPropertyChanged();
                }
            }
        }

        public int Id { get => id; private set => id = value; }

        public ListItem(int i, string t, string d, DateTimeOffset date, byte[] bytes, bool? Com = false)
        {
            Id = i;
            title = t;
            description = d;
            duedate = date;
            completed = Com;
            pixels = bytes ?? PictureHandler.DefaultPixels;
        }

    }
}
