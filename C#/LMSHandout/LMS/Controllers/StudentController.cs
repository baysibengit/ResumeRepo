using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using LMS.Models.LMSModels;
using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore.Storage.Json;

// For more information on enabling MVC for empty projects, visit https://go.microsoft.com/fwlink/?LinkID=397860

namespace LMS.Controllers
{
    [Authorize(Roles = "Student")]
    public class StudentController : Controller
    {
        private LMSContext db;
        public StudentController(LMSContext _db)
        {
            db = _db;
        }

        public IActionResult Index()
        {
            return View();
        }

        public IActionResult Catalog()
        {
            return View();
        }

        public IActionResult Class(string subject, string num, string season, string year)
        {
            ViewData["subject"] = subject;
            ViewData["num"] = num;
            ViewData["season"] = season;
            ViewData["year"] = year;
            return View();
        }

        public IActionResult Assignment(string subject, string num, string season, string year, string cat, string aname)
        {
            ViewData["subject"] = subject;
            ViewData["num"] = num;
            ViewData["season"] = season;
            ViewData["year"] = year;
            ViewData["cat"] = cat;
            ViewData["aname"] = aname;
            return View();
        }


        public IActionResult ClassListings(string subject, string num)
        {
            System.Diagnostics.Debug.WriteLine(subject + num);
            ViewData["subject"] = subject;
            ViewData["num"] = num;
            return View();
        }


        /*******Begin code to modify********/

        /// <summary>
        /// Returns a JSON array of the classes the given student is enrolled in.
        /// Each object in the array should have the following fields:
        /// "subject" - The subject abbreviation of the class (such as "CS")
        /// "number" - The course number (such as 5530)
        /// "name" - The course name
        /// "season" - The season part of the semester
        /// "year" - The year part of the semester
        /// "grade" - The grade earned in the class, or "--" if one hasn't been assigned
        /// </summary>
        /// <param name="uid">The uid of the student</param>
        /// <returns>The JSON array</returns>
        public IActionResult GetMyClasses(string uid)
        {
            var myClasses = from co in db.Courses 
                            join c in db.Classes on co.Id equals c.Course
                            join e in db.Enrollments on c.Id equals e.Class
                            where e.Student == uid 
                            select new 
                            {
                                subject = co.Dept,
                                number = co.Number,
                                name = co.Name,
                                season = c.Semester,
                                year = c.Year, 
                                grade = e.Grade == null ? "--": e.Grade
                            };
            return Json(myClasses.ToArray());
        }

        /// <summary>
        /// Returns a JSON array of all the assignments in the given class that the given student is enrolled in.
        /// Each object in the array should have the following fields:
        /// "aname" - The assignment name
        /// "cname" - The category name that the assignment belongs to
        /// "due" - The due Date/Time
        /// "score" - The score earned by the student, or null if the student has not submitted to this assignment.
        /// </summary>
        /// <param name="subject">The course subject abbreviation</param>
        /// <param name="num">The course number</param>
        /// <param name="season">The season part of the semester for the class the assignment belongs to</param>
        /// <param name="year">The year part of the semester for the class the assignment belongs to</param>
        /// <param name="uid"></param>
        /// <returns>The JSON array</returns>
        public IActionResult GetAssignmentsInClass(string subject, int num, string season, int year, string uid)
        { 
            var assignments = from co in db.Courses 
                              join c in db.Classes on co.Id equals c.Course
                              join cat in db.AssignmentCategories on c.Id equals cat.Class
                              join a in db.Assignments on cat.Id equals a.Category
                              join sub in db.Submissions on new { AssignmentID = a.Id, StudentID = uid } equals new { AssignmentID = sub.Assignment, StudentID = sub.Student } into submissions
                              from sub in submissions.DefaultIfEmpty()
                              where co.Dept == subject 
                                && co.Number == num 
                                && c.Semester == season 
                                && c.Year == year
                              select new 
                              {
                                  aname = a.Name,
                                  cname = cat.Name,
                                  due = a.Due,
                                  score = sub == null ? (uint?)null : sub.Score
                              };           
            return Json(assignments.ToArray());
        }

        /// <summary>
        /// Adds a submission to the given assignment for the given student
        /// The submission should use the current time as its DateTime
        /// You can get the current time with DateTime.Now
        /// The score of the submission should start as 0 until a Professor grades it
        /// If a Student submits to an assignment again, it should replace the submission contents
        /// and the submission time (the score should remain the same).
        /// </summary>
        /// <param name="subject">The course subject abbreviation</param>
        /// <param name="num">The course number</param>
        /// <param name="season">The season part of the semester for the class the assignment belongs to</param>
        /// <param name="year">The year part of the semester for the class the assignment belongs to</param>
        /// <param name="category">The name of the assignment category in the class</param>
        /// <param name="asgname">The new assignment name</param>
        /// <param name="uid">The student submitting the assignment</param>
        /// <param name="contents">The text contents of the student's submission</param>
        /// <returns>A JSON object containing {success = true/false}</returns>
        public IActionResult SubmitAssignmentText(string subject, int num, string season, int year,
          string category, string asgname, string uid, string contents)
        {
            var assignment = (from co in db.Courses
                              join c in db.Classes on co.Id equals c.Course
                              join cat in db.AssignmentCategories on c.Id equals cat.Class
                              join a in db.Assignments on cat.Id equals a.Category
                              where co.Dept == subject
                                 && co.Number == num
                                 && c.Semester == season
                                 && c.Year == year
                                 && cat.Name == category
                                 && a.Name == asgname
                              select a).FirstOrDefault();

            // Check if assignment exists and if it is still valid (not past due date)
            if (assignment == null || assignment.Due < DateTime.Now) {
                return Json(new { success = false, message = "Assignment not found or past due date" });
            }

            // Check if a submission already exists for the student and assignment
            var existingSubmission = (from sub in db.Submissions
                                      join a in db.Assignments on sub.Assignment equals a.Id
                                      join co in db.Courses on a.Id equals co.Id
                                      join c in db.Classes on co.Id equals c.Course
                                      where a.Id == assignment.Id
                                        && sub.Student == uid
                                        && co.Dept == subject
                                        && co.Number == num
                                        && c.Semester == season
                                        && c.Year == year
                                      select sub).FirstOrDefault();
            if (existingSubmission != null) {

                // Update existing submission
                existingSubmission.Time = DateTime.Now;
                existingSubmission.Contents = contents;
                db.SaveChanges();
            } else {
                Submission submission = new Submission {
                    Score = 0,
                    Time = DateTime.Now,
                    Contents = contents,
                    Student = uid,
                    Assignment = assignment.Id
                };
                db.Submissions.Add(submission);
                db.SaveChanges();
            }  
            return Json(new { success = true }); 
        }

        /// <summary>
        /// Enrolls a student in a class.
        /// </summary>
        /// <param name="subject">The department subject abbreviation</param>
        /// <param name="num">The course number</param>
        /// <param name="season">The season part of the semester</param>
        /// <param name="year">The year part of the semester</param>
        /// <param name="uid">The uid of the student</param>
        /// <returns>A JSON object containing {success = {true/false}. 
        /// false if the student is already enrolled in the class, true otherwise.</returns>
        public IActionResult Enroll(string subject, int num, string season, int year, string uid)
        {
            // Extract classID
            var classID = (from co in db.Courses 
                         join c in db.Classes on co.Id equals c.Course
                          where co.Dept == subject
                            && co.Number == num 
                            && c.Semester == season 
                            && c.Year == year
                         select c.Id).FirstOrDefault();
            // Check if successful 
            if ( classID <= 0 ) {
                return Json(new { success = false, message = "ClassID not found"});
            }

            // Check if the student is already enrolled in the class
            var existingEnrollment = (from e in db.Enrollments
                                      where e.Class == classID
                                        && e.Student == uid
                                      select e).FirstOrDefault();

            if ( existingEnrollment != null  ) {
                return Json(new { success = false, message = "Enrollment already exists"});
            }

            Enrollment newEnrollment = new Enrollment {
                Grade = "--",
                Student = uid,
                Class = classID
            }; 
            db.Enrollments.Add(newEnrollment);
            db.SaveChanges();
            return Json(new { success = true });          
        }

        /// <summary>
        /// Calculates a student's GPA
        /// A student's GPA is determined by the grade-point representation of the average grade in all their classes.
        /// Assume all classes are 4 credit hours.
        /// If a student does not have a grade in a class ("--"), that class is not counted in the average.
        /// If a student is not enrolled in any classes, they have a GPA of 0.0.
        /// Otherwise, the point-value of a letter grade is determined by the table on this page:
        /// https://advising.utah.edu/academic-standards/gpa-calculator-new.php
        /// </summary>
        /// <param name="uid">The uid of the student</param>
        /// <returns>A JSON object containing a single field called "gpa" with the number value</returns>
        public IActionResult GetGPA(string uid)
        {
            var grades = (from s in db.Students 
                          join e in db.Enrollments on s.Id equals e.Student
                          where e.Student == uid
                          select e.Grade).ToList(); 

            List<double> gradeValues = new List<double>();
            foreach ( var letterGrade in grades ) {

                 switch (letterGrade) 
                {
                    case "A":
                        gradeValues.Add(4.0);
                        break;
                    case "A-":
                        gradeValues.Add(3.7);
                        break;
                    case "B+":
                        gradeValues.Add(3.3);
                        break;
                    case "B":
                        gradeValues.Add(3.0);
                        break;
                    case "B-":
                        gradeValues.Add(2.7);
                        break;
                    case "C+":
                        gradeValues.Add(2.3);
                        break;
                    case "C":
                        gradeValues.Add(2.0);
                        break;
                    case "C-":
                        gradeValues.Add(1.7);
                        break;
                    case "D+":
                        gradeValues.Add(1.3);
                        break;
                    case "D":
                        gradeValues.Add(1.0);
                        break;
                    case "D-":
                        gradeValues.Add(0.7);
                        break;
                    case "E":
                        gradeValues.Add(0.0);
                        break;
                    default:
                        break;
                }
            }
            double gpa = 0.0;
            if (gradeValues.Count > 0)
            {
                gpa = gradeValues.Average();
            }
            return Json(new { gpa });
        }
                
        /*******End code to modify********/

    }
}

