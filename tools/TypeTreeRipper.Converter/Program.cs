
using System.Diagnostics.CodeAnalysis;
using Spectre.Console.Cli;
using TypeTreeRipper.Converter.Features.StructDump;
using TypeTreeRipper.Converter.Features.Tpk;

namespace TypeTreeRipper.Converter;

public sealed class Program
{
    [DynamicDependency(DynamicallyAccessedMemberTypes.All, typeof(StructDumpCommand))]
    [DynamicDependency(DynamicallyAccessedMemberTypes.All, typeof(TpkCommand))]
    [DynamicDependency(DynamicallyAccessedMemberTypes.All, "Spectre.Console.Cli.ExplainCommand", "Spectre.Console.Cli")]
    [DynamicDependency(DynamicallyAccessedMemberTypes.All, "Spectre.Console.Cli.VersionCommand", "Spectre.Console.Cli")]
    [DynamicDependency(DynamicallyAccessedMemberTypes.All, "Spectre.Console.Cli.XmlDocCommand", "Spectre.Console.Cli")]
    [DynamicDependency(DynamicallyAccessedMemberTypes.All, "Spectre.Console.Cli.OpenCliGeneratorCommand", "Spectre.Console.Cli")]
    [DynamicDependency(DynamicallyAccessedMemberTypes.All, typeof(EmptyCommandSettings))]
    [UnconditionalSuppressMessage("AOT", "IL3050:Calling members annotated with 'RequiresDynamicCodeAttribute' may break functionality when AOT compiling.", Justification = "With the above dependencies it works")]
    public static int Main(string[] args)
    {
        var app = new CommandApp();

        app.Configure(config =>
        {
            config.AddCommand<StructDumpCommand>("struct-dump").WithDescription("Converts a .ttbin to a StructDump file.");
            config.AddCommand<TpkCommand>("tpk").WithDescription("Converts a folder of versioned .ttbin files to a TPK.");
            config.PropagateExceptions();
        });

        return app.Run(args);
    }
}
