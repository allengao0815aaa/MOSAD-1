using System;
using toDoList.Models;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Data;

namespace toDoList.Common
{
    /// <summary>
    /// Line和Completed绑定
    /// </summary>
    public class VisibilityConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            bool? completed = value as bool?;
            if (completed != true)
                return Visibility.Collapsed;
            else
                return Visibility.Visible;
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            throw new NotImplementedException();
        }
    }

    /// <summary>
    /// CreateOrUpdate按钮的内容与SeletedItem绑定
    /// </summary>
    public class ContentConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            if (value is ListItem seleted)
                return "Update";
            else
                return "Create";
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            throw new NotImplementedException();
        }
    }

    public class DateConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            DateTimeOffset date = (DateTimeOffset)value;
            return date.ToString("d");
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            throw new NotImplementedException();
        }
    }
}
