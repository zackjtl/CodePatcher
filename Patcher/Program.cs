using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.IO;
using System.Text.RegularExpressions;
using System.Threading;
using DynamicExpresso;

namespace Patcher
{
    class Program
    {
        static void __Main(string[] args)
        {
            Console.WriteLine("Hello World!!");
        }

        public static string BaseDir = "";
        public static string JsonFilePath = "config.json";
        public static string AppExeDir = Directory.GetCurrentDirectory();
        public static int SVN = -1;
        static void Main(string[] args)
        {
            ParseArgs(args);

            Console.WriteLine("Base Directory: " + Directory.GetCurrentDirectory() + "\\" + BaseDir);

            List<ModificationConfig> configs = LoadConfigs(JsonFilePath);

            foreach (var action in configs)
            {
                if (action.TargetString == null)
                    action.TargetString = "";

                // Check if the condition true or false, if false, do not execute this action
                if (action.Condition != null && action.Condition != "")
                {
                    if (SVN != -1)
                    {
                        var interpreter = new Interpreter();
                        bool result = interpreter.Eval<bool>(action.Condition, new Parameter("svn", SVN));

                        if (result == false)
                            continue;
                    }
                }

                // The path for searching target file
                string path = Directory.GetCurrentDirectory() + "\\" + (action.Path == "" ? BaseDir : action.Path);

                string targetFilePath = FindFile(path, action.TargetFile);
                if (targetFilePath == null)
                {
                    Console.WriteLine($" !!!! [Error] Target file not found: {action.TargetFile} !!!!");
                    continue;
                }

                // The path for the new content file
                action.NewContentFile = AppExeDir + "\\" + action.NewContentFile;

                switch (action.Mode)
                {
                    case 1:
                        Console.WriteLine($" ********** Mode 1: Replace by file [{Path.GetFileName(targetFilePath)}] **********");
                        Console.WriteLine($" Target File: {targetFilePath}\n Target Function: {action.TargetFunction}\n Start String: {action.StartString}\n End String: {action.EndString}\n New Content File: {action.NewContentFile}");
                        ReplaceByFile(
                            targetFilePath,
                            action.TargetFunction,
                            action.StartString,
                            action.EndString,
                            action.NewContentFile);

                        Console.WriteLine("");
                        break;
                    case 2:
                        Console.WriteLine($" ********** Mode 2: Insert String [{Path.GetFileName(targetFilePath)}] **********");
                        Console.WriteLine($" Target File: {targetFilePath}\n Target Function: {action.TargetFunction}\n Target String: {action.TargetString}\n New String: {action.NewString}\n Insert After: {action.InsertAfter}");
                        InsertString(
                            targetFilePath,
                            action.TargetFunction,
                            action.TargetString,
                            action.NewString,
                            action.InsertAfter);

                        Console.WriteLine("");
                        break;
                    case 3:
                        Console.WriteLine($" ********** Mode 3: Replace String [{Path.GetFileName(targetFilePath)}] **********");
                        Console.WriteLine($" Target File: {targetFilePath}\n Target Function: {action.TargetFunction}\n Target String: {action.TargetString}\n New String: {action.NewString}");
                        ReplaceString(
                            targetFilePath,
                            action.TargetFunction,
                            action.TargetString,
                            action.NewString);
                        Console.WriteLine("");
                        break;
                    case 4:
                        Console.WriteLine($" ********** Mode 4: Append Content [{Path.GetFileName(targetFilePath)}] **********");
                        Console.WriteLine($" Target File: {targetFilePath}, New Content File: {action.NewContentFile}");
                        AppendContentToEndOfFile(
                            targetFilePath,
                            action.NewContentFile);
                        Console.WriteLine("");
                        break;
                    case 5:
                        Console.WriteLine($" ********** Mode 5: Replace File [{Path.GetFileName(targetFilePath)}] **********");
                        Console.WriteLine($" Target File: {targetFilePath}, Replacement File: {action.NewContentFile}");
                        ReplaceFileWithAnother(
                            targetFilePath,
                            action.NewContentFile);

                        Console.WriteLine("");
                        break;
                    default:
                        Console.WriteLine("Invalid mode specified in the configuration.");
                        break;
                }
            }
        }

        class ModificationConfig
        {
            public int Mode { get; set; }
            public string TargetFile { get; set; }
            public string TargetFunction { get; set; }
            public string StartString { get; set; }
            public string EndString { get; set; }
            public string NewContentFile { get; set; }
            public string TargetString { get; set; }
            public string NewString { get; set; }
            public bool InsertAfter { get; set; }
            public string Path { get; set; }
            public string Condition { get; set; }
        }
        static void ParseArgs(string[] args)
        {
            for (int i = 0; i < args.Length;)
            {
                if (args[i] == "-b")
                {
                    BaseDir = args[i + 1];
                    ++i;
                }
                if (args[i] == "-f")
                {
                    JsonFilePath = args[i + 1];
                    ++i;
                }
                if (args[i] == "-v")
                {
                    int.TryParse(args[i + 1], out SVN);
                    ++i;
                }
                if (args[i] == "-h")
                    ShowHelp();
                ++i;
            }
            Console.WriteLine("[Program Arguments]");
            Console.WriteLine($" Base Dir  : {BaseDir}");
            Console.WriteLine($" JSON File : {JsonFilePath}");

            if (SVN == -1)
                Console.WriteLine($" SVN       : Not set");
            else
                Console.WriteLine($" SVN       : {SVN}");
        }
        static void ShowHelp()
        {
            Console.WriteLine("Arguments: ");
            Console.WriteLine(" -b [path]   : Set default base directory, default: directory of this executable");
            Console.WriteLine(" -f [path]   : Indicate json file path, default: config.json");
            Console.WriteLine(" -v [number] : SVN Number, default: -1");
        }

        static List<ModificationConfig> LoadConfigs(string jsonFilePath)
        {
            string jsonContent = File.ReadAllText(jsonFilePath);
            return JsonConvert.DeserializeObject<List<ModificationConfig>>(jsonContent);
        }

        static string FindFile(string path, string fileName)
        {
            var files = Directory.GetFiles(path, fileName, SearchOption.AllDirectories);

            if (files.Length >= 1)
                return files[0];

            else
                return "";
        }

        static void ReplaceByFile(string targetFile, string targetFunction, string startString, string endString, string newContentFile)
        {
            string newContent = File.ReadAllText(newContentFile);
            string fileContent = File.ReadAllText(targetFile);

            if (!string.IsNullOrEmpty(targetFunction))
            {
                string functionPattern = $@"{Regex.Escape(targetFunction)}\s*\(.*?\)\s*\{{";
                Regex functionRegex = new Regex(functionPattern, RegexOptions.Singleline);
                Match functionMatch = functionRegex.Match(fileContent);

                if (functionMatch.Success)
                {
                    int functionStartIndex = functionMatch.Index;
                    int functionEndIndex = FindMatchingBrace(fileContent, functionStartIndex);

                    if (functionEndIndex != -1)
                    {
                        ////string functionContent = fileContent.Substring(functionStartIndex, functionEndIndex - functionStartIndex + 1);
                        int rangeStart = -1, rangeEnd = -1;

                        FindRangeOfPattern(fileContent, startString, endString, ref rangeStart, ref rangeEnd, functionStartIndex);
                        fileContent = fileContent.Remove(rangeStart, rangeEnd - rangeStart + 1).Insert(rangeStart, newContent);

                        ////ReplaceContentWithinRange(ref functionContent, startString, endString, newContent);
                        ////fileContent = fileContent.Remove(functionStartIndex, functionEndIndex - functionStartIndex + 1).Insert(functionStartIndex, functionContent);
                        File.WriteAllText(targetFile, fileContent);
                        Console.WriteLine($"  Content replaced successfully in {targetFile}.");
                    }
                }
            }
            else
            {
                ReplaceContentWithinRange(ref fileContent, startString, endString, newContent);
                File.WriteAllText(targetFile, fileContent);
                Console.WriteLine($"  Content replaced successfully in {targetFile}.");
            }
        }


        static void FindRangeOfPattern(string content, string startPattern, string endPattern, ref int rangeStart, ref int rangeEnd, int startIndex)
        {
            int start = content.IndexOf(startPattern, startIndex);
            rangeStart = start;

            if (start != 0)
            {
                int prevEndl = FindPreviousNewline(content, start);

                if (prevEndl != -1)
                    rangeStart = prevEndl + 1;
            }

            int end = content.IndexOf(endPattern, start);

            rangeEnd = content.IndexOf('\n', end);

            if (rangeEnd == -1)
                rangeEnd = end;
        }
        static int FindPreviousNewline(string text, int startIndex)
        {
            for (int i = startIndex; i >= 0; i--)
            {
                if (text[i] == '\n')
                {
                    return i;
                }
            }
            return -1; // 如果没有找到换行符，返回 -1
        }
        static int FindMatchingBrace(string content, int startIndex)
        {
            int braceCount = 0;
            for (int i = startIndex; i < content.Length; i++)
            {
                if (content[i] == '{')
                {
                    braceCount++;
                }
                else if (content[i] == '}')
                {
                    braceCount--;
                    if (braceCount == 0)
                    {
                        return i;
                    }
                }
            }
            return -1; // No matching closing brace found
        }

        static void ReplaceContentWithinRange(ref string content, string startString, string endString, string newContent)
        {
            string pattern = $@"({Regex.Escape(startString)})(.*?){Regex.Escape(endString)}";
            Regex regex = new Regex(pattern, RegexOptions.Singleline);

            if (regex.IsMatch(content))
            {
                content = regex.Replace(content, $"$1{newContent}$3");
            }
            else
            {
                Console.WriteLine("  !!! [Warning] Specified strings not found.!!!");
            }
        }

        static void InsertString(string targetFile, string targetFunction, string targetString, string newString, bool insertAfter)
        {
            string fileContent = File.ReadAllText(targetFile);

            if (!string.IsNullOrEmpty(targetFunction))
            {
                string functionPattern = $@"{Regex.Escape(targetFunction)}\s*\(.*?\)\s*\{{";
                Regex functionRegex = new Regex(functionPattern, RegexOptions.Singleline);
                Match functionMatch = functionRegex.Match(fileContent);

                if (functionMatch.Success)
                {
                    int functionStartIndex = functionMatch.Index;
                    int functionEndIndex = FindMatchingBrace(fileContent, functionStartIndex);

                    if (functionEndIndex != -1)
                    {
                        string functionContent = fileContent.Substring(functionStartIndex, functionEndIndex - functionStartIndex + 1);
                        functionContent = InsertOrReplaceLine(functionContent, targetString, newString, insertAfter);
                        fileContent = fileContent.Remove(functionStartIndex, functionEndIndex - functionStartIndex + 1).Insert(functionStartIndex, functionContent);
                        File.WriteAllText(targetFile, fileContent);
                        Console.WriteLine($"  String inserted successfully in {targetFile}.");
                    }
                }
            }
            else
            {
                int targetIndex = fileContent.IndexOf(targetString);

                if (targetIndex != -1)
                {
                    string updatedContent = InsertOrReplaceLine(fileContent, targetString, newString, insertAfter);
                    File.WriteAllText(targetFile, updatedContent);
                    Console.WriteLine($"  String inserted successfully in {targetFile}.");
                }
                else
                {
                    Console.WriteLine($"  !!! [Warning] Target string not found in {targetFile}.!!!");
                }
            }
        }

        static string InsertOrReplaceLine(string content, string targetString, string newString, bool insertAfter)
        {
            int targetIndex = content.IndexOf(targetString);

            if (targetIndex != -1)
            {
                if (insertAfter)
                {
                    int nextLineIdx = content.IndexOf('\n', targetIndex);

                    if (nextLineIdx == -1)
                    {
                        nextLineIdx = content.Length;
                        newString = "\n" + newString;
                    }

                    return content.Insert(nextLineIdx, newString);
                }
                else
                {
                    int prevLineEnd = FindPreviousNewline(content, targetIndex);

                    if (prevLineEnd == -1)
                    {
                        prevLineEnd = 0;
                    }
                    newString = newString;
                    return content.Insert(prevLineEnd, newString);
                }
                /*
                return insertAfter
                    ? content.Insert(targetIndex + targetString.Length, "\n" + newString)
                    : content.Insert(targetIndex, newString + "\n");*/
            }
            else
            {
                Console.WriteLine("  !!! [WARNING]Target string not found.!!!");
                return content;
            }
        }

        static void ReplaceString(string targetFile, string targetFunction, string targetString, string newString)
        {
            string fileContent = File.ReadAllText(targetFile);

            if (!string.IsNullOrEmpty(targetFunction))
            {
                string functionPattern = $@"{Regex.Escape(targetFunction)}\s*\(.*?\)\s*\{{";
                Regex functionRegex = new Regex(functionPattern, RegexOptions.Singleline);
                Match functionMatch = functionRegex.Match(fileContent);

                if (functionMatch.Success)
                {
                    int functionStartIndex = functionMatch.Index;
                    int functionEndIndex = FindMatchingBrace(fileContent, functionStartIndex);

                    if (functionEndIndex != -1)
                    {
                        string functionContent = fileContent.Substring(functionStartIndex, functionEndIndex - functionStartIndex + 1);
                        functionContent = ReplaceLine(functionContent, targetString, newString);
                        fileContent = fileContent.Remove(functionStartIndex, functionEndIndex - functionStartIndex + 1).Insert(functionStartIndex, functionContent);
                        File.WriteAllText(targetFile, fileContent);
                        Console.WriteLine($"  String replaced successfully in {targetFile}.");
                    }
                }
            }
            else
            {
                fileContent = ReplaceLine(fileContent, targetString, newString);
                File.WriteAllText(targetFile, fileContent);
                Console.WriteLine($"  String replaced successfully in {targetFile}.");
            }
        }

        static string ReplaceLine(string content, string targetString, string newString)
        {
            string pattern = $@"^.*{Regex.Escape(targetString)}.*$";
            Regex regex = new Regex(pattern, RegexOptions.Multiline);

            if (regex.IsMatch(content))
            {
                return regex.Replace(content, newString);
            }
            else
            {
                Console.WriteLine("  !!! [Warning] Target string not found. !!!");
                return content;
            }
        }

        static void AppendContentToEndOfFile(string targetFile, string newContentFile)
        {
            string newContent = File.ReadAllText(newContentFile);
            File.AppendAllText(targetFile, "\n" + newContent);
            Console.WriteLine($"  Content appended successfully to {targetFile}.");
        }

        static void ReplaceFileWithAnother(string targetFile, string replacementFile)
        {
            if (File.Exists(replacementFile))
            {
                const int NumberOfRetries = 3;
                const int DelayOnRetry = 1000;

                for (int i = 1; i <= NumberOfRetries; ++i)
                {
                    try
                    {
                        File.Copy(replacementFile, targetFile, true);
                        break; // When done we can break loop
                    }
                    catch (IOException e) when (i <= NumberOfRetries)
                    {
                        // You may check error code to filter some exceptions, not every error
                        // can be recovered.
                        Thread.Sleep(DelayOnRetry);
                    }
                }


                Console.WriteLine($"  File replaced successfully: {targetFile}.");
            }
            else
            {
                Console.WriteLine($"  !!! [WARNING] Replacement file does not exist: {replacementFile}.!!!");
            }
        }
    }
}
