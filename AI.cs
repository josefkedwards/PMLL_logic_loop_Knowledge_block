using System;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.Azure.WebJobs;
using Microsoft.Azure.WebJobs.Extensions.DurableTask;
using Microsoft.Extensions.Logging;
using OpenAI_API;

namespace CopilotAzureChatGPT5o
{
    public static class AIAgentOrchestrator
    {
        [FunctionName("AIAgentOrchestrator")]
        public static async Task Run(
            [OrchestrationTrigger] IDurableOrchestrationContext context,
            ILogger log)
        {
            // Retrieve short-term memory (STM) or initialize
            var shortTermMemory = context.GetInput<List<string>>() ?? new List<string>();

            // Wait for a new novel input via an external event
            string novelInput = await context.WaitForExternalEvent<string>("NovelInput");
            shortTermMemory.Add(novelInput);
            log.LogInformation($"[Orchestrator] Received novel input: {novelInput}");

            // Process the input using OpenAI via an activity function
            string openAiResponse = await context.CallActivityAsync<string>("ProcessNovelInputActivity", novelInput);
            log.LogInformation($"[Orchestrator] OpenAI response: {openAiResponse}");

            // Check if STM needs to be consolidated into LTM
            if (shortTermMemory.Count >= 5 || context.CurrentUtcDateTime.Subtract(context.StartTime).TotalMinutes >= 10)
            {
                await context.CallActivityAsync("ConsolidateMemory", shortTermMemory);
                log.LogInformation("[Orchestrator] STM consolidated into LTM.");
                shortTermMemory.Clear();
            }

            // Prevent excessive history buildup by using ContinueAsNew
            context.ContinueAsNew(shortTermMemory);
        }
    }

    public static class ProcessNovelInputActivity
    {
        [FunctionName("ProcessNovelInputActivity")]
        public static async Task<string> Run(
            [ActivityTrigger] string novelInput,
            ILogger log)
        {
            string apiKey = Environment.GetEnvironmentVariable("OPENAI_API_KEY");
            if (string.IsNullOrEmpty(apiKey))
            {
                log.LogError("OpenAI API key is not set in environment variables.");
                return "Error: No API key";
            }

            var openAiClient = new OpenAIAPI(apiKey);
            var completionRequest = new OpenAI_API.Completions.CompletionRequest
            {
                Prompt = $"Process this input and provide insights: {novelInput}",
                MaxTokens = 100,
                Temperature = 0.7
            };

            try
            {
                var result = await openAiClient.Completions.CreateCompletionAsync(completionRequest);
                string response = result.Completions?[0].Text.Trim() ?? "No response";
                log.LogInformation($"[ProcessNovelInputActivity] Processed input: {novelInput} | Response: {response}");
                return response;
            }
            catch (Exception ex)
            {
                log.LogError($"[ProcessNovelInputActivity] OpenAI API error: {ex.Message}");
                return "Error processing input";
            }
        }
    }

    public static class ConsolidateMemory
    {
        [FunctionName("ConsolidateMemory")]
        public static async Task Run(
            [ActivityTrigger] List<string> memoryBatch,
            ILogger log)
        {
            log.LogInformation("[ConsolidateMemory] Consolidating STM into LTM.");
            foreach (var item in memoryBatch)
            {
                log.LogInformation($" - {item}");
            }
            await Task.Delay(500); // Simulate database write
        }
    }

using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Microsoft.Azure.WebJobs;
using Microsoft.Azure.WebJobs.Extensions.DurableTask;
using Microsoft.Extensions.Logging;
using Azure.Data.Tables;
using System.Linq;
using OpenAI_API;

namespace CopilotAzureChatGPT5o
{
    public static class SelfLearningAI
    {
        private const string TableName = "AIMemoryTable";
        private static TableClient _tableClient;

        [FunctionName("InitializeMemory")]
        public static async Task InitializeMemory([TimerTrigger("0 */30 * * * *")] TimerInfo myTimer, ILogger log)
        {
            var connectionString = Environment.GetEnvironmentVariable("AZURE_STORAGE_CONNECTION_STRING");
            _tableClient = new TableClient(connectionString, TableName);
            await _tableClient.CreateIfNotExistsAsync();
            log.LogInformation("[Memory] AI Memory Initialized.");
        }

        [FunctionName("StoreMemory")]
        public static async Task StoreMemory([ActivityTrigger] MemoryEntry memory, ILogger log)
        {
            await _tableClient.AddEntityAsync(memory);
            log.LogInformation($"[Memory] Stored: {memory.Input} → {memory.Response}");
        }

        [FunctionName("RetrieveMemory")]
        public static async Task<List<MemoryEntry>> RetrieveMemory([ActivityTrigger] string query, ILogger log)
        {
            var results = _tableClient.QueryAsync<MemoryEntry>(m => m.Input.Contains(query));
            List<MemoryEntry> memoryEntries = new();
            await foreach (var entry in results) memoryEntries.Add(entry);
            log.LogInformation($"[Memory] Retrieved {memoryEntries.Count} relevant memories.");
            return memoryEntries;
        }

        [FunctionName("ProcessUserQuery")]
        public static async Task<string> ProcessUserQuery([OrchestrationTrigger] IDurableOrchestrationContext context, ILogger log)
        {
            string userInput = context.GetInput<string>();
            log.LogInformation($"[AI] Processing input: {userInput}");

            // Retrieve past similar queries from memory
            var previousResponses = await context.CallActivityAsync<List<MemoryEntry>>("RetrieveMemory", userInput);
            if (previousResponses.Any())
            {
                var bestMatch = previousResponses.OrderByDescending(m => m.Timestamp).First();
                log.LogInformation($"[Memory] Found past response: {bestMatch.Response}");
                return bestMatch.Response;
            }

            // No memory match, call OpenAI API
            string aiResponse = await context.CallActivityAsync<string>("ProcessWithOpenAI", userInput);
            log.LogInformation($"[AI] OpenAI Response: {aiResponse}");

            // Store new knowledge
            var newMemory = new MemoryEntry { PartitionKey = "AI_Memory", RowKey = Guid.NewGuid().ToString(), Input = userInput, Response = aiResponse };
            await context.CallActivityAsync("StoreMemory", newMemory);

            return aiResponse;
        }

        [FunctionName("ProcessWithOpenAI")]
        public static async Task<string> ProcessWithOpenAI([ActivityTrigger] string input, ILogger log)
        {
            string apiKey = Environment.GetEnvironmentVariable("OPENAI_API_KEY");
            if (string.IsNullOrEmpty(apiKey))
            {
                log.LogError("OpenAI API key is missing.");
                return "Error: No API key.";
            }

            var openAiClient = new OpenAIAPI(apiKey);
            var completionRequest = new OpenAI_API.Completions.CompletionRequest
            {
                Prompt = $"Learn from the following input and respond: {input}",
                MaxTokens = 100,
                Temperature = 0.7
            };

            try
            {
                var result = await openAiClient.Completions.CreateCompletionAsync(completionRequest);
                string response = result.Completions?[0].Text.Trim() ?? "No response";
                log.LogInformation($"[AI] Generated response: {response}");
                return response;
            }
            catch (Exception ex)
            {
                log.LogError($"[AI] OpenAI API error: {ex.Message}");
                return "Error processing input.";
            }
        }
    }

    public class MemoryEntry : ITableEntity
    {
        public string PartitionKey { get; set; }
        public string RowKey { get; set; }
        public string Input { get; set; }
        public string Response { get; set; }
        public DateTimeOffset Timestamp { get; set; }
        public string ETag { get; set; }
    }
}

