using System.ComponentModel;
using Spectre.Console;
using Spectre.Console.Cli;

namespace TypeTreeRipper.Converter.Features.Tpk;

public sealed class TpkCommand : Command<TpkCommand.Settings>
{
    public sealed class Settings : CommandSettings
    {
        [CommandArgument(0, "<input-folder>")]
        [Description("The folder to convert. Must contain subfolders named after unity versions containing .ttbin files.")]
        public required string InputFolder { get; init; }

        [CommandArgument(1, "<output-path>")]
        [Description("The output .tpk file path, or the directory to place TPKs into.")]
        public required string OutputPath { get; init; }

        [CommandOption("-a|--all")]
        [Description("If a TPK should be emitted for each compression format.")]
        public required bool EmitAllCompressions { get; init; }
    }

    protected override int Execute(CommandContext context, Settings settings, CancellationToken cancellationToken)
    {
        if (settings.EmitAllCompressions)
        {
            Directory.CreateDirectory(settings.OutputPath);
        }

        TpkOutput.Output(Path.GetFullPath(settings.InputFolder), Path.GetFullPath(settings.OutputPath),
            settings.EmitAllCompressions);

        return 0;
    }

    protected override ValidationResult Validate(CommandContext context, Settings settings)
    {
        if (!Directory.Exists(settings.InputFolder))
        {
            return ValidationResult.Error($"Input folder '{settings.InputFolder}' does not exist.");
        }

        if (!settings.EmitAllCompressions && Directory.Exists(settings.OutputPath))
        {
            return ValidationResult.Error($"Output path '{settings.OutputPath}' is a directory.");
        }

        if (settings.EmitAllCompressions && !Directory.Exists(settings.OutputPath))
        {
            return ValidationResult.Error($"Output path '{settings.OutputPath}' is not a directory.");
        }

        return base.Validate(context, settings);
    }
}
