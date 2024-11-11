import sqlite3
import time

# Connect to the database
conn = sqlite3.connect('venice.db')
cursor = conn.cursor()

# Create the table for session tracking
cursor.execute('''
    CREATE TABLE IF NOT EXISTS sessions (
        session_id INTEGER PRIMARY KEY,
        flag_state TEXT,
        conversation_log TEXT,
        timestamp TEXT
    );
''')

# Function to insert session data
def insert_session_data(session_id, flag_state, conversation_log, timestamp):
    cursor.execute('''
        INSERT INTO sessions (session_id, flag_state, conversation_log, timestamp)
        VALUES (?, ?, ?, ?);
    ''', (session_id, flag_state, conversation_log, timestamp))
    conn.commit()

# Function to retrieve session data based on session ID
def retrieve_session_data(session_id):
    cursor.execute('''
        SELECT flag_state, conversation_log, timestamp
        FROM sessions
        WHERE session_id = ?;
    ''', (session_id,))
    return cursor.fetchone()

# Function to simulate a conversation and store session data
def handle_new_session(session_id, flag_state, conversation_log):
    timestamp = time.strftime('%Y-%m-%d %H:%M:%S')
    insert_session_data(session_id, flag_state, conversation_log, timestamp)

# Function to check and update session logic
def session_logic(session_id):
    # Retrieve the most recent session data
    session_data = retrieve_session_data(session_id)
    if session_data:
        flag_state, conversation_log, timestamp = session_data
        print(f"Restored session {session_id}:")
        print(f"Flag State: {flag_state}")
        print(f"Last Conversation: {conversation_log}")
        print(f"Timestamp: {timestamp}")
        # Here you can add further logic to process flags and context
    else:
        print(f"No previous session found for session ID {session_id}. Starting fresh.")

# Example session simulation
session_id = 1
flag_state = "reset_context=False, check_flags=False"
conversation_log = "Started the conversation with some context."

# Handle new session and store data
handle_new_session(session_id, flag_state, conversation_log)

# Simulate retrieving a session and processing it
session_logic(session_id)

# Close the database connection
conn.close()
