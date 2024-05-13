using Esri.ArcGISRuntime;
using Esri.ArcGISRuntime.Maui;
using Microsoft.Extensions.Logging;

namespace App
{
    public static class MauiProgram
    {
        public static MauiApp CreateMauiApp()
        {
            var builder = MauiApp.CreateBuilder();
            builder
                .UseMauiApp<App>()
                .ConfigureFonts(fonts =>
                {
                    fonts.AddFont("OpenSans-Regular.ttf", "OpenSansRegular");
                    fonts.AddFont("OpenSans-Semibold.ttf", "OpenSansSemibold");
                });
            builder.UseArcGISRuntime(config => config.UseApiKey("AAPK5f2fdeaa48b54fd68385f2c86c3c0a3dLI3rzK79Et-fYWZw663B7Y-OXbeQNsu4nZg02Bjo7frdnMjSgDHAH3_gybezL-YY"));
            builder.Services.AddSingleton<IGeolocation>(Geolocation.Default);

#if DEBUG
            builder.Logging.AddDebug();
#endif

            return builder.Build();
        }
    }
}
