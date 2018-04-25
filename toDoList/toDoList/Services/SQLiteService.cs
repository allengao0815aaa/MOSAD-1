using SQLitePCL;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using toDoList.Common;
using toDoList.Models;
using toDoList.ViewModels;
using Windows.UI.Xaml.Media.Imaging;

namespace toDoList.Services
{
    class SQLiteService
    {
        private static string DB = "toDoList.db";
        private static string TABLE = "ListItems";

        public static void InitializeDatabase()
        {
            SQLiteConnection conn = new SQLiteConnection(DB);
            string CREATE_TABLE = "CREATE TABLE IF NOT EXISTS " + TABLE + "(" +
                                                    "id INTEGER PRIMARY KEY," +
                                                    "title TEXT, " +
                                                    "description TEXT, " +
                                                    "duedate TEXT, " +
                                                    "image BLOB," +
                                                    "isCompleted INTEGER)";
            using (var statement = conn.Prepare(CREATE_TABLE))
            {
                statement.Step();
            }
        }

        public static void Insert(int id, string title, string description, DateTimeOffset duedate, byte[] pixels, bool? completed)
        {
            SQLiteConnection conn = new SQLiteConnection(DB);
            string INSERT = "INSERT INTO " + TABLE + " VALUES(?, ?, ?, ?, ?, ?)";
            using (var statement = conn.Prepare(INSERT))
            {
                statement.Bind(1, id);
                statement.Bind(2, title);
                statement.Bind(3, description);
                statement.Bind(4, duedate.ToString());
                statement.Bind(5, pixels);
                statement.Bind(6, Convert.ToInt32(completed));
                statement.Step();
            }
        }

        public static void SetComplete(int id, bool? completed)
        {
            SQLiteConnection conn = new SQLiteConnection(DB);
            string UPDATE = "UPDATE " + TABLE + " " +
                "SET isCompleted = (?) " +
                "WHERE ID = (?)";
            using (var statement = conn.Prepare(UPDATE))
            {
                statement.Bind(1, Convert.ToInt32(completed));
                statement.Bind(2, id);
                statement.Step();
            }
        }

        public static void Delete(int id)
        {
            SQLiteConnection conn = new SQLiteConnection(DB);
            string DELETE = "DELETE FROM " + TABLE + " WHERE id = (?)";
            using (var statement = conn.Prepare(DELETE))
            {
                statement.Bind(1, id);
                statement.Step();
            }
        }

        public static void Update(int id, string title, string description, DateTimeOffset duedate, byte[] pixels, bool? completed)
        {
            SQLiteConnection conn = new SQLiteConnection(DB);
            string UPDATE = "UPDATE " + TABLE + " " +
                "SET title = (?), description = (?), duedate = (?), image = (?), isCompleted = (?) " +
                "WHERE ID = (?)";
            using (var statement = conn.Prepare(UPDATE))
            {
                statement.Bind(1, title);
                statement.Bind(2, description);
                statement.Bind(3, duedate.ToString());
                statement.Bind(4, pixels);
                statement.Bind(5, Convert.ToInt32(completed));
                statement.Bind(6, id);
                statement.Step();
            }
        }

        public static void Load()
        {
            SQLiteConnection conn = new SQLiteConnection(DB);
            string GETALL = "SELECT * FROM " + TABLE;
            ListItemViewModels all = VM.Vm;
            using (var statement = conn.Prepare(GETALL))
            {
                while (SQLiteResult.ROW == statement.Step())
                {
                    int id = Convert.ToInt32((Int64)statement[0]);
                    string title = statement[1] as string;
                    string description = statement[2] as string;
                    DateTimeOffset duedate = DateTimeOffset.Parse(statement[3] as string);
                    byte[] pixels = statement[4] as byte[];
                    bool? completed = Convert.ToBoolean(statement[5]);

                    ListItem item = new ListItem(id, title, description, duedate, pixels, completed);
                    all.Items.Add(item);
                    ListItemViewModels.count = id;
                }
                ++ListItemViewModels.count;
                TileService.Reset();
            }
        }
    }
}
