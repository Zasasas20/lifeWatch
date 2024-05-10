

using System.Xml.Linq;

namespace App
{
    public partial class MainPage : ContentPage
    {

        public MainPage()
        {
            InitializeComponent();
        }

        private Border buildTextBox()
        {   
            Border border = new Border();
            border.Stroke = Brush.Orange;
            border.StrokeThickness = 3;
            Entry entry = new Entry();
            entry.AutomationId = "TextBox";
            entry.Placeholder = "Enter Device ID: ";
            entry.PlaceholderColor = Colors.Gray;
            entry.TextColor = Colors.Black;
            entry.Completed += add_entry;
            border.Content = entry;
            return border;
        }

        private Button buildTextButton()
        {
            Button button = new Button();
            button.Text = "Add";
            button.BackgroundColor = Colors.Orange;
            button.Clicked += add_button;
            button.Margin = new Thickness(5,0,0,0);
            return button;
        }

        private FlexLayout buildTextFlexLayout(Border border, Button button) 
        {
            FlexLayout flexLayout = new FlexLayout();
            flexLayout.Margin = new Thickness(5,5,5,5);
            flexLayout.Children.Add(border);
            flexLayout.Children.Add(button);
            flexLayout.SetGrow(flexLayout[0], 1);
            return flexLayout;
        }

        private Label buildLabel(String text, String value)
        {
            Label label = new Label();
            label.Text = text + value;
            label.TextColor = Colors.Black;
            label.FontSize = 26;
            return label;
        }

        private StackLayout buildDeviceText(String ID, String name)
        {
            StackLayout stack = new StackLayout();
            stack.Children.Add(buildLabel("Device ID: ", ID));
            stack.Children.Add(buildLabel("Name: ", name));
            stack.Children.Add(buildLabel("Status: ", "Status"));
            return stack;
        }

        private StackLayout buildImageButtons(String ID)
        {
            StackLayout stack = new StackLayout();

            ImageButton img = new ImageButton();
            img.Source = "pendant.png";
            img.MaximumHeightRequest = 75;
            img.MaximumWidthRequest = 75;
            img.Clicked += viewDetails;
            img.AutomationId = ID;

            Button button = new Button();
            button.Text = "Remove";
            button.BackgroundColor = Colors.Orange;
            button.MaximumWidthRequest = 100;
            button.Clicked += remove_device;
            button.Margin = new Thickness(0, 5, 0, 0);

            stack.Children.Add(img);
            stack.Children.Add(button);

            return stack;
        }

        private void remove_device(object? sender, EventArgs e)
        {
            Border element = (Border)((FlexLayout)((StackLayout)((Button)sender).Parent).Parent).Parent;
            ((StackLayout)(element.Parent)).Remove(element);
        }

        private void create_pendant(String ID)
        {
            StackLayout devices = (StackLayout)((ScrollView)((FlexLayout)FindByName("devices"))[0]).Content;
            String Name = "bruh";

            FlexLayout layout = new FlexLayout();
            layout.Children.Add(buildDeviceText(ID, Name));
            layout.Children.Add(buildImageButtons(ID));
            layout.Margin = new Thickness(5, 5, 5, 5);
            layout.SetGrow(layout[0], 1);
            Border border = new Border();
            border.Stroke = Brush.Orange;
            border.StrokeThickness = 3;
            border.Content = layout;
            border.Margin = new Thickness(10, 5, 5, 10);
            devices.Add(border);
        }

        private void add_button(object? sender, EventArgs e)
        {
            String ID = ((Entry)((Border)((FlexLayout)((Button)sender).Parent)[0]).Content).Text;
            create_pendant(ID);
        }

        private void add_entry(object? sender, EventArgs e)
        {
            create_pendant(((Entry)sender).Text);
        }

        private void plus_button(object sender, EventArgs e)
        {
            Grid items = (Grid)FindByName("items");
            ImageButton button = (ImageButton)FindByName("addT");
            if (items.Children.Count > 3)
            {
                items.Children.Remove(items.Children[3]);
                button.Source = "plus.png";
            }
            else
            {
                FlexLayout layout = buildTextFlexLayout(buildTextBox(), buildTextButton());
                items.SetRow(layout, 1);
                items.Children.Add(layout);
                button.Source = "minus.png";
            }
        }

        private void viewDetails(object? sender, EventArgs e)
        {
            Navigation.PushAsync(new TrackingPage(((ImageButton)sender).AutomationId));
        }
    }

}
