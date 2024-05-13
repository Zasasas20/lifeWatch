using Esri.ArcGISRuntime.Geometry;
using Esri.ArcGISRuntime.Location;
using Esri.ArcGISRuntime.Mapping;
using Esri.ArcGISRuntime.Navigation;
using Esri.ArcGISRuntime.Symbology;
using Esri.ArcGISRuntime.Tasks.NetworkAnalysis;
using Esri.ArcGISRuntime.UI;

using Color = System.Drawing.Color;
using Location = Esri.ArcGISRuntime.Location.Location;

namespace App;

public partial class TrackingPage : ContentPage
{

	String id;

	public TrackingPage(String ID)
	{
		InitializeComponent();
		id = ID;
		((Label)FindByName("device")).Text = "Name: " + id;
        Initialize();
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
        ((Entry)(FindByName("textInput"))).Text = "";

    }

    // Variables for tracking the navigation route.
    private RouteTracker _tracker;
    private RouteResult _routeResult;
    private Route _route;

    // List of driving directions for the route.
    private IReadOnlyList<DirectionManeuver> _directionsList;

    // Graphics to show progress along the route.
    private Graphic _routeAheadGraphic;
    private Graphic _routeTraveledGraphic;

    // Source coordinates 
    private MapPoint _origin;

    // Destination coordinates
    private readonly MapPoint _destination = new MapPoint(6.891163102, 52.218665792, SpatialReferences.Wgs84);

    // Routing servie for world
    private readonly Uri _routingUri = new Uri("https://route-api.arcgis.com/arcgis/rest/services/World/Route/NAServer/Route_World");
   
    private CancellationTokenSource _cancelTokenSource;
    private bool _isCheckingLocation;

    public async Task GetCurrentLocation()
    {
        try
        {
            _isCheckingLocation = true;

            GeolocationRequest request = new GeolocationRequest(GeolocationAccuracy.Best, TimeSpan.FromSeconds(10));

            _cancelTokenSource = new CancellationTokenSource();

            Microsoft.Maui.Devices.Sensors.Location location = await Geolocation.Default.GetLocationAsync(request, _cancelTokenSource.Token);


            if (location != null)
                _origin = new MapPoint(location.Longitude, location.Latitude, SpatialReferences.Wgs84);
        }
        catch (Exception ex)
        {
            Console.WriteLine(ex);

        }
        finally
        {
            _isCheckingLocation = false;
        }
    }

    public void CancelRequest()
    {
        if (_isCheckingLocation && _cancelTokenSource != null && _cancelTokenSource.IsCancellationRequested == false)
            _cancelTokenSource.Cancel();
    }
    private async Task Initialize()
    {
        try
        {
            // Create the map view.
            MyMapView.Map = new Esri.ArcGISRuntime.Mapping.Map(BasemapStyle.ArcGISStreets);

            // Wait for current location
            await GetCurrentLocation();

            // Create the route task, using the online routing service.
            RouteTask routeTask = await RouteTask.CreateAsync(_routingUri);

            // Get the default route parameters.
            RouteParameters routeParams = await routeTask.CreateDefaultParametersAsync();

            // Explicitly set values for parameters.
            routeParams.ReturnDirections = true;
            routeParams.ReturnStops = true;
            routeParams.ReturnRoutes = true;
            routeParams.OutputSpatialReference = SpatialReferences.Wgs84;

            // Create stops for each location.
            Stop stop1 = new Stop(_origin) { Name = "Origin" };
            Stop stop2 = new Stop(_destination) { Name = "Destination" };

            // Assign the stops to the route parameters.
            List<Stop> stopPoints = new List<Stop> { stop1, stop2 };
            routeParams.SetStops(stopPoints);

            // Get the route results.
            _routeResult = await routeTask.SolveRouteAsync(routeParams);
            _route = _routeResult.Routes[0];

            // Add a graphics overlay for the route graphics.
            MyMapView.GraphicsOverlays.Add(new GraphicsOverlay());

            // Add graphics for the stops.
            SimpleMarkerSymbol stopSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle.Diamond, Color.OrangeRed, 20);
            MyMapView.GraphicsOverlays[0].Graphics.Add(new Graphic(_origin, stopSymbol));
            MyMapView.GraphicsOverlays[0].Graphics.Add(new Graphic(_destination, stopSymbol));


            // Create a graphic (with a dashed line symbol) to represent the route.
            _routeAheadGraphic = new Graphic(_route.RouteGeometry) { Symbol = new SimpleLineSymbol(SimpleLineSymbolStyle.Dash, Color.BlueViolet, 5) };

            // Create a graphic (solid) to represent the route that's been traveled (initially empty).
            _routeTraveledGraphic = new Graphic { Symbol = new SimpleLineSymbol(SimpleLineSymbolStyle.Solid, Color.LightBlue, 3) };

            // Add the route graphics to the map view.
            MyMapView.GraphicsOverlays[0].Graphics.Add(_routeAheadGraphic);
            MyMapView.GraphicsOverlays[0].Graphics.Add(_routeTraveledGraphic);

            // Set the map viewpoint to show the entire route.
            await MyMapView.SetViewpointGeometryAsync(_route.RouteGeometry, 100);

            // Enable the navigation button.
            StartNavigationButton.IsEnabled = true;
        }
        catch (Exception e)
        {
            await Application.Current.MainPage.DisplayAlert("Error", e.Message, "OK");
        }
    }

    private void StartNavigation(object sender, EventArgs e)
    {
        // Disable the start navigation button.
        StartNavigationButton.IsEnabled = false;

        // Get the directions for the route.
        _directionsList = _route.DirectionManeuvers;

        // Create a route tracker.
        _tracker = new RouteTracker(_routeResult, 0, true);

        // Handle route tracking status changes.
        //_tracker.TrackingStatusChanged += TrackingStatusUpdated;

        // Turn on navigation mode for the map view.
        MyMapView.LocationDisplay.AutoPanMode = LocationDisplayAutoPanMode.Recenter;
        MyMapView.LocationDisplay.AutoPanModeChanged += AutoPanModeChanged;

        // Add a data source for the location display.
        //MyMapView.LocationDisplay.DataSource = new RouteTrackerLocationDataSource(_tracker, new SystemLocationDataSource());

        // Enable the location display (this wil start the location data source).
        MyMapView.LocationDisplay.IsEnabled = true;

        // Show the message block for text output.
        MessagesTextBlock.IsVisible = false;
    }

    private void TrackingStatusUpdated(object sender, RouteTrackerTrackingStatusChangedEventArgs e)
    {
        TrackingStatus status = e.TrackingStatus;

        // Start building a status message for the UI.
        System.Text.StringBuilder statusMessageBuilder = new System.Text.StringBuilder();

        // Check the destination status.
        if (status.DestinationStatus == DestinationStatus.NotReached || status.DestinationStatus == DestinationStatus.Approaching)
        {
            statusMessageBuilder.AppendLine("Distance remaining: " +
                                        status.RouteProgress.RemainingDistance.DisplayText + " " +
                                        status.RouteProgress.RemainingDistance.DisplayTextUnits.PluralDisplayName);

            statusMessageBuilder.AppendLine("Time remaining: " +
                                            status.RouteProgress.RemainingTime.ToString(@"hh\:mm\:ss"));

            if (status.CurrentManeuverIndex + 1 < _directionsList.Count)
            {
                statusMessageBuilder.AppendLine("Next direction: " + _directionsList[status.CurrentManeuverIndex + 1].DirectionText);
            }

            // Set geometries for progress and the remaining route.
            _routeAheadGraphic.Geometry = status.RouteProgress.RemainingGeometry;
            _routeTraveledGraphic.Geometry = status.RouteProgress.TraversedGeometry;
        }
        else if (status.DestinationStatus == DestinationStatus.Reached)
        {
            statusMessageBuilder.AppendLine("Destination reached.");

            // Set the route geometries to reflect the completed route.
            _routeAheadGraphic.Geometry = null;
            _routeTraveledGraphic.Geometry = status.RouteResult.Routes[0].RouteGeometry;

            // Navigate to the next stop (if there are stops remaining).
            if (status.RemainingDestinationCount > 1)
            {
                _tracker.SwitchToNextDestinationAsync();
            }

            // Stop the simulated location data source.
            MyMapView.LocationDisplay.DataSource.StopAsync();
        }

        Microsoft.Maui.ApplicationModel.MainThread.BeginInvokeOnMainThread(() =>
        {
            // Show the status information in the UI.
            MessagesTextBlock.Text = statusMessageBuilder.ToString().TrimEnd('\n').TrimEnd('\r');
        });
    }

    private void AutoPanModeChanged(object sender, LocationDisplayAutoPanMode e)
    {
        // Turn the recenter button on or off when the location display changes to or from navigation mode.
        RecenterButton.IsEnabled = e != LocationDisplayAutoPanMode.Recenter;
    }

    private void RecenterButton_Click(object sender, EventArgs e)
    {
        // Change the mapview to use navigation mode.
        MyMapView.LocationDisplay.AutoPanMode = LocationDisplayAutoPanMode.Recenter;
    }

    public void Dispose()
    {
        // Stop the tracker.
        if (_tracker != null)
        {
            _tracker.TrackingStatusChanged -= TrackingStatusUpdated;
            _tracker = null;
        }

        // Stop the location data source.
        MyMapView.LocationDisplay?.DataSource?.StopAsync();
    }
}

public class RouteTrackerDisplayLocationDataSource : LocationDataSource
{
    private LocationDataSource _inputDataSource;
    private RouteTracker _routeTracker;

    public RouteTrackerDisplayLocationDataSource(LocationDataSource dataSource, RouteTracker routeTracker)
    {
        // Set the data source
        _inputDataSource = dataSource ?? throw new ArgumentNullException(nameof(dataSource));

        // Set the route tracker.
        _routeTracker = routeTracker ?? throw new ArgumentNullException(nameof(routeTracker));

        // Change the tracker location when the source location changes.
        _inputDataSource.LocationChanged += InputLocationChanged;

        // Update the location output when the tracker location updates.
        _routeTracker.TrackingStatusChanged += TrackingStatusChanged;
    }

    private void InputLocationChanged(object sender, Location e)
    {
        // Update the tracker location with the new location from the source (simulation or GPS).
        _routeTracker.TrackLocationAsync(e);
    }

    private void TrackingStatusChanged(object sender, RouteTrackerTrackingStatusChangedEventArgs e)
    {
        // Check if the tracking status has a location.
        if (e.TrackingStatus.DisplayLocation != null)
        {
            // Call the base method for LocationDataSource to update the location with the tracked (snapped to route) location.
            UpdateLocation(e.TrackingStatus.DisplayLocation);
        }
    }

    protected override Task OnStartAsync() => _inputDataSource.StartAsync();

    protected override Task OnStopAsync() => _inputDataSource.StartAsync();
}

