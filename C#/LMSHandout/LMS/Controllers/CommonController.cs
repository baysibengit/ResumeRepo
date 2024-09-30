﻿using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text.Json;
using System.Threading.Tasks;
// using AspNetCore;
using LMS.Models.LMSModels;
using Microsoft.AspNetCore.Http.Connections;
using Microsoft.AspNetCore.Mvc;
using NuGet.Protocol;

// For more information on enabling MVC for empty projects, visit https://go.microsoft.com/fwlink/?LinkID=397860

namespace LMS.Controllers
{
    public class CommonController : Controller
    {
        private readonly LMSContext db;

        public CommonController(LMSContext _db)
        {
            db = _db;
        }

        /*******Begin code to modify********/

        /// <summary>
        /// Retreive a JSON array of all departments from the database.
        /// Each object in the array should have a field called "name" and "subject",
        /// where "name" is the department name and "subject" is the subject abbreviation.
        /// </summary>
        /// <returns>The JSON array</returns>
        public IActionResult GetDepartments()
        {  
            var query = from d in db.Departments 
                        select new 
                            {
                                name = d.Name,
                                subject = d.Id
                            };      
            return Json(query.ToArray());
        }



        /// <summary>
        /// Returns a JSON array representing the course catalog.
        /// Each object in the array should have the following fields:
        /// "subject": The subject abbreviation, (e.g. "CS")
        /// "dname": The department name, as in "Computer Science"
        /// "courses": An array of JSON objects representing the courses in the department.
        ///            Each field in this inner-array should have the following fields:
        ///            "number": The course number (e.g. 5530)
        ///            "cname": The course name (e.g. "Database Systems")
        /// </summary>
        /// <returns>The JSON array</returns>
        public IActionResult GetCatalog()
        {       
            var query = from d in db.Departments
                        select new 
                        {
                            subject = d.Id,
                            dname = d.Name,
                            courses = ( from co in db.Courses 
                                        where d.Id == co.Dept
                                        select new 
                                        {
                                            number = co.Number,
                                            cname = co.Name
                                        }).ToArray()
                        };     
            return Json(query.ToArray());
        }

        /// <summary>
        /// Returns a JSON array of all class offerings of a specific course.
        /// Each object in the array should have the following fields:
        /// "season": the season part of the semester, such as "Fall"
        /// "year": the year part of the semester
        /// "location": the location of the class
        /// "start": the start time in format "hh:mm:ss"
        /// "end": the end time in format "hh:mm:ss"
        /// "fname": the first name of the professor
        /// "lname": the last name of the professor
        /// </summary>
        /// <param name="subject">The subject abbreviation, as in "CS"</param>
        /// <param name="number">The course number, as in 5530</param>
        /// <returns>The JSON array</returns>
        public IActionResult GetClassOfferings(string subject, int number)
        {
            var query = from co in db.Courses 
                        join c in db.Classes on co.Id equals c.Course
                        join p in db.Professors on c.Professor equals p.Id
                        where co.Dept == subject 
                            && co.Number == number
                        select new 
                            {
                                season = c.Semester,
                                year = c.Year,
                                location = c.Location,
                                start = c.StartTime,
                                end = c.EndTime,
                                fname = p.First,
                                lname = p.Last
                            }
                            ;            
            return Json(query.ToArray());
        }

        /// <summary>
        /// This method does NOT return JSON. It returns plain text (containing html).
        /// Use "return Content(...)" to return plain text.
        /// Returns the contents of an assignment.
        /// </summary>
        /// <param name="subject">The course subject abbreviation</param>
        /// <param name="num">The course number</param>
        /// <param name="season">The season part of the semester for the class the assignment belongs to</param>
        /// <param name="year">The year part of the semester for the class the assignment belongs to</param>
        /// <param name="category">The name of the assignment category in the class</param>
        /// <param name="asgname">The name of the assignment in the category</param>
        /// <returns>The assignment contents</returns>
        public IActionResult GetAssignmentContents(string subject, int num, string season, int year, string category, string asgname)
        {
            var instructions = (from co in db.Courses 
                        join c in db.Classes on co.Id equals c.Course
                        join cat in db.AssignmentCategories on c.Id equals cat.Class
                        join a in db.Assignments on cat.Id equals a.Category
                        where co.Dept == subject 
                            && co.Number == num 
                            && c.Semester == season 
                            && c.Year == year 
                            && cat.Name == category 
                            && a.Name == asgname
                        select a.Instructions).FirstOrDefault(); 

            if ( !string.IsNullOrEmpty(instructions) ) {        
                return Content(instructions);
            }
            return Content("Unable to retrieve assignment contents");
        }


        /// <summary>
        /// This method does NOT return JSON. It returns plain text (containing html).
        /// Use "return Content(...)" to return plain text.
        /// Returns the contents of an assignment submission.
        /// Returns the empty string ("") if there is no submission.
        /// </summary>
        /// <param name="subject">The course subject abbreviation</param>
        /// <param name="num">The course number</param>
        /// <param name="season">The season part of the semester for the class the assignment belongs to</param>
        /// <param name="year">The year part of the semester for the class the assignment belongs to</param>
        /// <param name="category">The name of the assignment category in the class</param>
        /// <param name="asgname">The name of the assignment in the category</param>
        /// <param name="uid">The uid of the student who submitted it</param>
        /// <returns>The submission text</returns>
        public IActionResult GetSubmissionText(string subject, int num, string season, int year, string category, string asgname, string uid)
        {  
            var submissionContents = (from co in db.Courses 
                                      join c in db.Classes on co.Id equals c.Course
                                      join cat in db.AssignmentCategories on c.Id equals cat.Class 
                                      join a in db.Assignments on cat.Id equals a.Category
                                      join sub in db.Submissions on a.Id equals sub.Assignment
                                      join s in db.Students on sub.Student equals s.Id 
                                      where co.Dept == subject 
                                          && co.Number == num
                                          && c.Semester == season 
                                          && c.Year == year 
                                          && cat.Name == category 
                                          && a.Name == asgname  
                                          && s.Id == uid 
                                      select sub.Contents).FirstOrDefault();  
            
            if ( !string.IsNullOrEmpty(submissionContents) ) {        
                return Content(submissionContents);
            }
            return Content("");
        }


        /// <summary>
        /// Gets information about a user as a single JSON object.
        /// The object should have the following fields:
        /// "fname": the user's first name
        /// "lname": the user's last name
        /// "uid": the user's uid
        /// "department": (professors and students only) the name (such as "Computer Science") of the department for the user. 
        ///               If the user is a Professor, this is the department they work in.
        ///               If the user is a Student, this is the department they major in.    
        ///               If the user is an Administrator, this field is not present in the returned JSON
        /// </summary>
        /// <param name="uid">The ID of the user</param>
        /// <returns>
        /// The user JSON object 
        /// or an object containing {success: false} if the user doesn't exist
        /// </returns>
        public IActionResult GetUser(string uid)
        {  
            // Extract potential student
            var studentQuery = from s in db.Students 
                          join d in db.Departments on s.Major equals d.Id
                          where s.Id == uid 
                          select new 
                                {
                                    fname = s.First,
                                    lname = s.Last,
                                    uid = s.Id, 
                                    department = s.Major
                                };
            var student = studentQuery.FirstOrDefault();
            if ( student != null ) {
                return Json(student);
            }

            // Extract potential professor  
            var professorQuery = from p in db.Professors
                            join d in db.Departments on p.Dept equals d.Id 
                            where p.Id == uid 
                            select new 
                                {
                                    fname = p.First,
                                    lname = p.Last, 
                                    uid = p.Id,
                                    department = p.Dept
                                };
            var professor = professorQuery.FirstOrDefault();
            if ( professor != null  ) {
                return Json(professor);
            }

            var administratorQuery = from a in db.Administrators
                                     where a.Id == uid 
                                     select new 
                                        {
                                            fname = a.First,
                                            lname = a.Last, 
                                            uid = a.Id
                                        };
            var administrator = administratorQuery.FirstOrDefault();
            if ( administrator != null ) {
                return Json(administrator);
            }            
            return Json(new { success = false, message = "Unable to get user" });
        }
        /*******End code to modify********/
    }
}

