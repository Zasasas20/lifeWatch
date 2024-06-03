namespace App;

public partial class LoadingPage
{
    public LoadingPage()
    {
        InitializeComponent();
    }

    public void ShowLoading()
    {
        CheckMarkAnimation.IsVisible = false;
        XMarkAnimation.IsVisible = false;
        LoadingIndicator.IsRunning = true;
        LoadingIndicator.IsVisible = true;
        LoadingLabel.Text = "Loading, please wait...";
    }

    public void ShowCheckMark()
    {
        LoadingIndicator.IsRunning = false;
        LoadingIndicator.IsVisible = false;
        XMarkAnimation.IsVisible = false;
        CheckMarkAnimation.IsVisible = true;
        LoadingLabel.Text = "Data Loaded Successfully";
    }

    public void ShowXMark()
    {
        LoadingIndicator.IsRunning = false;
        LoadingIndicator.IsVisible = false;
        CheckMarkAnimation.IsVisible = false;
        XMarkAnimation.IsVisible = true;
        LoadingLabel.Text = "No Data Available";
    }
}