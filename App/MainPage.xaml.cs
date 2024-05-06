namespace App
{
    public partial class MainPage : ContentPage
    {

        public MainPage()
        {
            InitializeComponent();
        }

        private Entry buildTextBox()
        {
            Entry entry = new Entry();
            entry.AutomationId = "TextBox";
            entry.Placeholder = "Enter Device ID: ";
            entry.PlaceholderColor = Colors.Gray;
            entry.TextColor = Colors.Black;
            return entry;
        }

        private Button buildTextButton()
        {
            Button button = new Button();
            button.Text = "Add";
            button.BackgroundColor = Colors.Orange;
            button.Clicked += add_button;
            return button;
        }

        private FlexLayout buildTextFlexLayout(Entry entry, Button button) 
        {
            FlexLayout flexLayout = new FlexLayout();
            flexLayout.Margin = new Thickness(5,5,5,5);
            flexLayout.Children.Add(entry);
            flexLayout.Children.Add(button);
            flexLayout.SetGrow(flexLayout[0], 1);
            return flexLayout;
        }

        private Label buildLabel(String text, String value)
        {
            Label label = new Label();
            label.Text = text + value;
            label.TextColor = Colors.Black;
            label.FontSize = 18;
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

        private StackLayout buildImageButtons()
        {
            StackLayout stack = new StackLayout();

            ImageButton img = new ImageButton();
            img.Source = "pendant.png";
            img.MaximumHeightRequest = 50;
            img.MaximumWidthRequest = 50;

            Button button = new Button();
            button.Text = "Remove";
            button.BackgroundColor = Colors.Orange;
            button.MaximumWidthRequest = 100;
            button.Clicked += remove_device;

            stack.Children.Add(img);
            stack.Children.Add(button);

            return stack;
        }

        private void remove_device(object? sender, EventArgs e)
        {
            FlexLayout element = (FlexLayout)(((StackLayout)((Button)sender).Parent).Parent);
            ((StackLayout)(element.Parent)).Children.Remove(element);
        }

        private void add_button(object? sender, EventArgs e)
        {
            StackLayout devices = (StackLayout)((FlexLayout)FindByName("devices"))[0];

            String ID = ((Entry)((FlexLayout)((Button)sender).Parent)[0]).Text;
            String Name = "bruh";

            FlexLayout layout = new FlexLayout();
            layout.Margin = new Thickness(25, 10, 50, 10);
            layout.Children.Add(buildDeviceText(ID, Name));
            layout.Children.Add(buildImageButtons());
            layout.SetGrow(layout[0], 1);
            devices.Children.Add(layout);
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
    }

}
