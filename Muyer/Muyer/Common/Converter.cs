using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Data;

namespace Muyer.Common
{
    class VolumeConverter: IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            return (double)value * 100;
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            return (double)value / 100;
        }
    }

    class MaxPositionConverter: IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            MediaElement element = value as MediaElement;
            return element.NaturalDuration.TimeSpan.TotalMilliseconds;
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            throw new NotImplementedException();
        }
    }
}
