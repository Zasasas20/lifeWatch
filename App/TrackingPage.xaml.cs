namespace App;

public partial class TrackingPage : ContentPage
{

	String id;

	public TrackingPage(String ID)
	{
		InitializeComponent();
		id = ID;
		((Label)FindByName("device")).Text = "Name: " + id;
	}

	private void close(object sender, EventArgs e)
	{
		Navigation.PopAsync();
	}

	private void SendText(object? sender, EventArgs e)
	{
        string Text = "(" + DateTime.Now.ToString("hh:mm:ss") + ") " + ((Entry)(FindByName("textInput"))).Text;
        Label textBox = (Label)FindByName("chatLog");
        textBox.Text = Text + "\n" + textBox.Text;
    }

}