using System.ComponentModel;
using Spectre.Console;
using Spectre.Console.Cli;
using TypeTreeRipper.BinaryFormat;

namespace TypeTreeRipper.Converter.Features.StructDump;

public sealed class StructDumpCommand : Command<StructDumpCommand.Settings>
{
    public sealed class Settings : CommandSettings
    {
        [CommandArgument(0, "<input-path>")]
        [Description("The path to the .ttbin file to read from.")]
        public required string InputPath { get; init; }

        [CommandArgument(1, "<output-path>")]
        [Description("The path to output the StructDump file to.")]
        public required string OutputPath { get; init; }

        [CommandOption("-b|--binary")]
        [Description("If the output should be binary instead of text.")]
        public required bool UseBinaryOutput { get; init; }
    }

    protected override int Execute(CommandContext context, Settings settings, CancellationToken cancellationToken)
    {
        var binary = TypeTreeBinary.FromFile(settings.InputPath);
        if (settings.UseBinaryOutput)
        {
            StructDumpBinaryOutput.Output(binary, settings.OutputPath);
        }
        else
        {
            StructDumpTextOutput.Output(binary, settings.OutputPath);
        }

        return 0;
    }

    protected override ValidationResult Validate(CommandContext context, Settings settings)
    {
        if (!File.Exists(settings.InputPath))
        {
            return ValidationResult.Error($"Input file '{settings.InputPath}' does not exist.");
        }

        if (Directory.Exists(settings.OutputPath))
        {
            return ValidationResult.Error($"Output path '{settings.OutputPath}' is an existing directory.");
        }

        return base.Validate(context, settings);
    }
}
