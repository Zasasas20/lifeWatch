using uPLibrary.Networking.M2Mqtt.Messages;
using uPLibrary.Networking.M2Mqtt;
using System.Xml.Linq;
using System.Text;
using System.Text.Json;
using System.Diagnostics;

namespace App
{
    public partial class MainPage : ContentPage
    {
        MqttClient client = new MqttClient("80.115.229.72");
        AppResponse response = new AppResponse();
        bool searching = false;
        string clientId = Guid.NewGuid().ToString();

        public MainPage()
        {
            InitializeComponent();
        }

        protected override void OnAppearing()
        {
            SetupBroker();
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

        async void client_MqttMsgPublishReceived(object sender, MqttMsgPublishEventArgs e)
        {
            if (e.Topic == "App/Init")
            {
                response = JsonSerializer.Deserialize<AppResponse>(e.Message);
                if (response.sessionID == clientId && response.req == "Response")
                {
                    searching = false;
                }
            }
            await Task.Yield();
        }

        async void SetupBroker()
        {
            client.MqttMsgPublishReceived += client_MqttMsgPublishReceived;
            client.Connect(clientId);
            client.Subscribe(new string[] { "Chip/Message", "App/Init" }, new byte[] { MqttMsgBase.QOS_LEVEL_AT_MOST_ONCE, MqttMsgBase.QOS_LEVEL_AT_MOST_ONCE });
            await Task.Yield();
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

        private StackLayout buildDeviceText(String ID, String status)
        {
            StackLayout stack = new StackLayout();
            stack.Children.Add(buildLabel("Device ID: ", ID));
            stack.Children.Add(buildLabel("Status: ", status));
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

        private void create_pendant(String ID, String status)
        {
            StackLayout devices = (StackLayout)((ScrollView)((FlexLayout)FindByName("devices"))[0]).Content;

            FlexLayout layout = new FlexLayout();
            layout.Children.Add(buildDeviceText(ID, status));
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

        async void pendantRequest(String ID)
        {
            Stopwatch stopwatch = new Stopwatch();
            App_Request request = new App_Request();
            request.code = ID;
            request.sessionID = clientId;
            request.req = "Request";
            client.Publish("App/Init", JsonSerializer.SerializeToUtf8Bytes<App_Request>(request));
            searching = true;
            stopwatch.Start();
            while (searching)
            {
                if (stopwatch.ElapsedMilliseconds > 3000)
                {
                    break;
                }
            }
            if (response.status != "NULL" && stopwatch.ElapsedMilliseconds < 3000)
            {
                create_pendant(response.code, response.status);
            }
            stopwatch.Stop();
        }

        private void add_button(object? sender, EventArgs e)
        {
            String ID = ((Entry)((Border)((FlexLayout)((Button)sender).Parent)[0]).Content).Text;
            pendantRequest(ID);
        }

        private void add_entry(object? sender, EventArgs e)
        {
            pendantRequest(((Entry)sender).Text);
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
            Navigation.PushAsync(new TrackingPage(((ImageButton)sender).AutomationId, clientId));
        }
    }

}
