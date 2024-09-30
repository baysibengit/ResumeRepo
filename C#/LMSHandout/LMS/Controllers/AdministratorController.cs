using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text.Json;
using System.Threading.Tasks;
using LMS.Models.LMSModels;
using Microsoft.AspNetCore.Mvc;

// For more information on enabling MVC for empty projects, visit https://go.microsoft.com/fwlink/?LinkID=397860

namespace LMS.Controllers
{
    public class AdministratorController : Controller
    {
        private readonly LMSContext db;

        public AdministratorController(LMSContext _db)
        {
            db = _db;
        }

        // GET: /<controller>/
        public IActionResult Index()
        {
            return View();
        }

        public IActionResult Department(string subject)
        {
            ViewData["subject"] = subject;
            return View();
        }

        public IActionResult Course(string subject, string num)
        {
            ViewData["subject"] = subject;
            ViewData["num"] = num;
            return View();
        }

        /*******Begin code to modify********/

        /// <summary>
        /// Create a department which is uniquely identified by it's subject code
        /// </summary>
        /// <param name="subject">the subject code</param>
        /// <param name="name">the full name of the department</param>
        /// <returns>A JSON object containing {success = true/false}.
        /// false if the department already exists, true otherwise.</returns>
        public IActionResult CreateDepartment(string subject, string name)
        {
            // Error Handle
            if ( string.IsNullOrEmpty(subject) || string.IsNullOrEmpty(name) ) {
                return Json(new { success = false, message = "Null or empty passed through subject or name" });

            }
            var department = from d in db.Departments
                        where d.Id  == subject
                            && d.Name == name
                        select d;

            if ( department.Any() ) {
                return Json(new { success = false, message = "Department already exists"});
            }
            else {
                Department newDept = new Department {
                Id = subject,
                Name = name
                };
                db.Departments.Add(newDept);
                db.SaveChanges();
                return Json(new { success = true, message = "Department successfully created"});
            }
        }

        /// <summary>
        /// Returns a JSON array of all the courses in the given department.
        /// Each object in the array should have the following fields:
        /// "number" - The course number (as in 5530)
        /// "name" - The course name (as in "Database Systems")
        /// </summary>
        /// <param name="subjCode">The department subject abbreviation (as in "CS")</param>
        /// <returns>The JSON result</returns>
        public IActionResult GetCourses(string subject)
        {
            var courses = from c in db.Courses 
                        where c.Dept == subject
                        select new 
                            {
                                number = c.Number,
                                name = c.Name
                            };      
            return Json(courses.ToArray());
        }

        /// <summary>
        /// Returns a JSON array of all the professors working in a given department.
        /// Each object in the array should have the following fields:
        /// "lname" - The professor's last name
        /// "fname" - The professor's first name
        /// "uid" - The professor's uid
        /// </summary>
        /// <param name="subject">The department subject abbreviation</param>
        /// <returns>The JSON result</returns>
        public IActionResult GetProfessors(string subject)
        {
            var professors = from p in db.Professors            
                        where p.Dept == subject
                        select new 
                            {
                                lname = p.Last,
                                fname = p.First,
                                uid = p.Id
                            };      
            return Json(professors.ToArray());
            
        }

        /// <summary>
        /// Creates a course.
        /// A course is uniquely identified by its number + the subject to which it belongs
        /// </summary>
        /// <param name="subject">The subject abbreviation for the department in which the course will be added</param>
        /// <param name="number">The course number</param>
        /// <param name="name">The course name</param>
        /// <returns>A JSON object containing {success = true/false}.
        /// false if the course already exists, true otherwise.</returns>
        public IActionResult CreateCourse(string subject, int number, string name)
        { 
            // Error handle
            if ( string.IsNullOrEmpty(subject) || string.IsNullOrEmpty(name) ) {
                return Json(new { success = false, message = "Null or empty passed through subject or name" });

            }

            var course = from c in db.Courses
                        where c.Number == number && c.Dept == subject  // Num and sub uniquely identify
                        select c;

            if ( course.Any() ) {
                return Json(new { success = false, message = "Course already exists"});
            }
            else {
                Course newCourse = new Course {
                Number = (short)number,
                Name = name, 
                Dept = subject
                };
                db.Courses.Add(newCourse);
                db.SaveChanges();
                return Json(new { success = true});
            }
        }

        /// <summary>
        /// Creates a class offering of a given course.
        /// </summary>
        /// <param name="subject">The department subject abbreviation</param>
        /// <param name="number">The course number</param>
        /// <param name="season">The season part of the semester</param>
        /// <param name="year">The year part of the semester</param>
        /// <param name="start">The start time</param>
        /// <param name="end">The end time</param>
        /// <param name="location">The location</param>
        /// <param name="instructor">The uid of the professor</param>
        /// <returns>A JSON object containing {success = true/false}. 
        /// false if another class occupies the same location during any time 
        /// within the start-end range in the same semester, or if there is already
        /// a Class offering of the same Course in the same Semester,
        /// true otherwise.</returns>
        public IActionResult CreateClass(string subject, int number, string season, int year, DateTime start, DateTime end, string location, string instructor)
        {
            // Error Handline for null parameters 
            if ( string.IsNullOrEmpty(subject) || string.IsNullOrEmpty(location) || string.IsNullOrEmpty(instructor) || year == 0 ) {
                return Json(new { success = false, message = "Null or empty passed through parameter" });
            }

            // Ensure class end time is after class start time 
            if (end < start) {
                return Json(new { success = false, message = "End time must be after start time" });
            }

            // Extract course ID
            var course = (from c in db.Courses 
                         where c.Number == number 
                            && c.Dept == subject
                         select c).FirstOrDefault();
             
            // Check if successful 
            if ( course == null ) {
                return Json(new { success = false, message = "Course not found"});
            }

            TimeOnly newStart = new TimeOnly(start.Hour, start.Minute, start.Second, start.Millisecond);
            TimeOnly newEnd = new TimeOnly(end.Hour, end.Minute, end.Second, end.Millisecond);
          
            var classesInSlot = from c in db.Classes
                                where c.Course == course.Id 
                                 && c.Year == year 
                                 && c.Semester == season 
                                 && ((newStart >= c.StartTime && newStart < c.EndTime) || (newEnd > c.StartTime && newEnd <= c.EndTime))
                                select c;
            
            if ( classesInSlot.Any() ) {
                return Json(new { success = false, message = "Another class occupies time in same location"});
            }
            else {
                Class newClass = new Class {
                Year = (uint)year,
                Semester = season, 
                Location = location,
                StartTime = newStart,
                EndTime = newEnd,
                Course = course.Id,
                Professor = instructor
                };
                db.Classes.Add(newClass);
                db.SaveChanges();
                return Json(new { success = true});
            }
        }
        /*******End code to modify********/

    }
}

