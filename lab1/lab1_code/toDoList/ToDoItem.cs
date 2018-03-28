using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Collections.ObjectModel;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media.Imaging;

namespace toDoList.ViewModel
{
    public class ToDoItem
    {
        public string title;
        public string description;
        public DateTime duedate;
        public BitmapImage bitmap;

        public ToDoItem(string t, string d, DateTime date, BitmapImage bi)
        {
            title = t;
            description = d;
            duedate = date;
            bitmap = bi;
        }
    }

    public class ToDoItems
    {
        static public ObservableCollection<ToDoItem> items = new ObservableCollection<ToDoItem>();
    }
}
