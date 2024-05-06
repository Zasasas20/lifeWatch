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

}