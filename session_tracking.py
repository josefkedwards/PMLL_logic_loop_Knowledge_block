import sqlite3
from datetime import datetime

# Connect to the database
def get_db_connection():
    return sqlite3.connect('venice.db')

# Create the table for session tracking
def create_sessions_table(cursor):
    """Create the sessions table if it doesn't exist."""
    cursor.execute('''
        CREATE TABLE IF NOT EXISTS sessions (
            session_id INTEGER PRIMARY KEY,
            flag_state TEXT,
            conversation_log TEXT,
            timestamp TEXT
        );
    ''')

# Function to insert session data
def insert_session_data(cursor, session_id, flag_state, conversation_log, timestamp):
    """Insert session data into the sessions table."""
    cursor.execute('''
        INSERT INTO sessions (session_id, flag_state, conversation_log, timestamp)
        VALUES (?, ?, ?, ?);
    ''', (session_id, flag_state, conversation_log, timestamp))

# Function to retrieve session data based on session ID
def retrieve_session_data(cursor, session_id):
    """Retrieve session data from the sessions table based on session ID."""
    cursor.execute('''
        SELECT flag_state, conversation_log, timestamp
        FROM sessions
        WHERE session_id = ?;
    ''', (session_id,))
    return cursor.fetchone()

# Function to simulate a conversation and store session data
def handle_new_session(cursor, session_id, flag_state, conversation_log):
    """Simulate a conversation and store session data."""
    timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    insert_session_data(cursor, session_id, flag_state, conversation_log, timestamp)

# Function to check and update session logic
def session_logic(cursor, session_id):
    """Check and update session logic."""
    session_data = retrieve_session_data(cursor, session_id)
    if session_data:
        flag_state, conversation_log, timestamp = session_data
        print(f"Restored session {session_id}:")
        print(f"Flag State: {flag_state}")
        print(f"Last Conversation: {conversation_log}")
        print(f"Timestamp: {timestamp}")
    else:
        print(f"No previous session found for session ID {session_id}. Starting fresh.")

# Example session simulation
def main():
    session_id = 1
    flag_state = "reset_context=False, check_flags=False"
    conversation_log = "Started the conversation with some context."

    with get_db_connection() as conn:
        cursor = conn.cursor()
        create_sessions_table(cursor)
        handle_new_session(cursor, session_id, flag_state, conversation_log)
        session_logic(cursor, session_id)

# Run the example
if __name__ == "__main__":
    main()
