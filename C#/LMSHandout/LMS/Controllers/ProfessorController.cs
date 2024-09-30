using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text.Json;
using System.Threading.Tasks;
using LMS.Models.LMSModels;
using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using Microsoft.CodeAnalysis.CSharp.Syntax;
using Microsoft.CodeAnalysis.FlowAnalysis.DataFlow.PointsToAnalysis;
using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Query.Internal;
using Microsoft.VisualBasic;

// For more information on enabling MVC for empty projects, visit https://go.microsoft.com/fwlink/?LinkID=397860

namespace LMS_CustomIdentity.Controllers
{
    [Authorize(Roles = "Professor")]
    public class ProfessorController : Controller
    {

        private readonly LMSContext db;

        public ProfessorController(LMSContext _db)
        {
            db = _db;
        }

        public IActionResult Index()
        {
            return View();
        }

        public IActionResult Students(string subject, string num, string season, string year)
        {
            ViewData["subject"] = subject;
            ViewData["num"] = num;
            ViewData["season"] = season;
            ViewData["year"] = year;
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

        public IActionResult Categories(string subject, string num, string season, string year)
        {
            ViewData["subject"] = subject;
            ViewData["num"] = num;
            ViewData["season"] = season;
            ViewData["year"] = year;
            return View();
        }

        public IActionResult CatAssignments(string subject, string num, string season, string year, string cat)
        {
            ViewData["subject"] = subject;
            ViewData["num"] = num;
            ViewData["season"] = season;
            ViewData["year"] = year;
            ViewData["cat"] = cat;
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

        public IActionResult Submissions(string subject, string num, string season, string year, string cat, string aname)
        {
            ViewData["subject"] = subject;
            ViewData["num"] = num;
            ViewData["season"] = season;
            ViewData["year"] = year;
            ViewData["cat"] = cat;
            ViewData["aname"] = aname;
            return View();
        }

        public IActionResult Grade(string subject, string num, string season, string year, string cat, string aname, string uid)
        {
            ViewData["subject"] = subject;
            ViewData["num"] = num;
            ViewData["season"] = season;
            ViewData["year"] = year;
            ViewData["cat"] = cat;
            ViewData["aname"] = aname;
            ViewData["uid"] = uid;
            return View();
        }

        /*******Begin code to modify********/


        /// <summary>
        /// Returns a JSON array of all the students in a class.
        /// Each object in the array should have the following fields:
        /// "fname" - first name
        /// "lname" - last name
        /// "uid" - user ID
        /// "dob" - date of birth
        /// "grade" - the student's grade in this class
        /// </summary>
        /// <param name="subject">The course subject abbreviation</param>
        /// <param name="num">The course number</param>
        /// <param name="season">The season part of the semester for the class the assignment belongs to</param>
        /// <param name="year">The year part of the semester for the class the assignment belongs to</param>
        /// <returns>The JSON array</returns>
        public IActionResult GetStudentsInClass(string subject, int num, string season, int year)
        {
             var query = from co in db.Courses
                join c in db.Classes on co.Id equals c.Course
                join e in db.Enrollments on c.Id equals e.Class
                join s in db.Students on e.Student equals s.Id
                where co.Dept == subject
                      && co.Number == num
                      && c.Semester == season
                      && c.Year == year
                select new
                {
                    fname = s.First,
                    lname = s.Last,
                    uid = s.Id,
                    dob = s.Dob,
                    grade = e.Grade
                };

            return Json(query.ToArray());             
        }



        /// <summary>
        /// Returns a JSON array with all the assignments in an assignment category for a class.
        /// If the "category" parameter is null, return all assignments in the class.
        /// Each object in the array should have the following fields:
        /// "aname" - The assignment name
        /// "cname" - The assignment category name.
        /// "due" - The due DateTime
        /// "submissions" - The number of submissions to the assignment
        /// </summary>
        /// <param name="subject">The course subject abbreviation</param>
        /// <param name="num">The course number</param>
        /// <param name="season">The season part of the semester for the class the assignment belongs to</param>
        /// <param name="year">The year part of the semester for the class the assignment belongs to</param>
        /// <param name="category">The name of the assignment category in the class, 
        /// or null to return assignments from all categories</param>
        /// <returns>The JSON array</returns>
        public IActionResult GetAssignmentsInCategory(string subject, int num, string season, int year, string category)
        {
            var query = from co in db.Courses 
                        join c in db.Classes on co.Id equals c.Course
                        join cat in db.AssignmentCategories on c.Id equals cat.Class 
                        join a in db.Assignments on cat.Id equals a.Category 
                        where co.Dept == subject 
                            && co.Number == num
                            && c.Semester == season 
                            && c.Year == year
                            && (category == null || cat.Name == category)
                        select new 
                            {
                                a.Id,
                                aname = a.Name,
                                cname = cat.Name,
                                due = a.Due, 
                            };
              var assignments = query.ToList().Select(a => new
              {
                a.aname,
                a.cname,
                a.due,
                submissions = db.Submissions.Count(s => s.Assignment == a.Id)
              });
            return Json(assignments.ToArray());
        }
        


        /// <summary>
        /// Returns a JSON array of the assignment categories for a certain class.
        /// Each object in the array should have the folling fields:
        /// "name" - The category name
        /// "weight" - The category weight
        /// </summary>
        /// <param name="subject">The course subject abbreviation</param>
        /// <param name="num">The course number</param>
        /// <param name="season">The season part of the semester for the class the assignment belongs to</param>
        /// <param name="year">The year part of the semester for the class the assignment belongs to</param>
        /// <param name="category">The name of the assignment category in the class</param>
        /// <returns>The JSON array</returns>
        public IActionResult GetAssignmentCategories(string subject, int num, string season, int year)
        {
            var query = from co in db.Courses 
                        join c in db.Classes on co.Id equals c.Course
                        join cat in db.AssignmentCategories on c.Id equals cat.Class
                        where co.Dept == subject
                            && co.Number == num 
                            && c.Semester == season 
                            && c.Year == year
                        select new 
                            {
                                name = cat.Name,
                                weight = cat.Weight
                            };

            return Json(query.ToArray());
        }

        /// <summary>
        /// Creates a new assignment category for the specified class.
        /// If a category of the given class with the given name already exists, return success = false.
        /// </summary>
        /// <param name="subject">The course subject abbreviation</param>
        /// <param name="num">The course number</param>
        /// <param name="season">The season part of the semester for the class the assignment belongs to</param>
        /// <param name="year">The year part of the semester for the class the assignment belongs to</param>
        /// <param name="category">The new category name</param>
        /// <param name="catweight">The new category weight</param>
        /// <returns>A JSON object containing {success = true/false} </returns>
        public IActionResult CreateAssignmentCategory(string subject, int num, string season, int year, string category, int catweight)
        {
            // Check for null category
            if ( string.IsNullOrEmpty(category)  || catweight == 0) {
                return Json(new { success = false, message = "Null or empty category" });
            }
            // Extract Class ID
            var classID = (from co in db.Courses
                         join c in db.Classes on co.Id equals c.Course 
                         where co.Dept == subject 
                            && co.Number == num 
                            && c.Semester == season 
                            && c.Year == year 
                         select c.Id).FirstOrDefault();

            // Check if successful 
            if ( classID <= 0 ) {
                return Json(new { success = false, message = "Class not found"});
            }

            
            // Check for existing category
            var existingCategory = (from cat in db.AssignmentCategories
                                    where cat.Class == classID 
                                        && cat.Name == category
                                    select cat).FirstOrDefault();

            if ( existingCategory != null ) {
                return Json(new { success = false, message = "Category already exists"});
            }
            
            AssignmentCategory newCat = new AssignmentCategory {
                Name = category,
                Weight = (byte)catweight,
                Class = classID
            };

            db.AssignmentCategories.Add(newCat);
            db.SaveChanges();
            return Json(new { success = true}); 
        }

        /// <summary>
        /// Creates a new assignment for the given class and category.
        /// A professor creates a new assignment. The grades of all students in the class should be updated. 
        /// Modify your CreateAssignment method to have this side-effect.
        /// </summary>
        /// <param name="subject">The course subject abbreviation</param>
        /// <param name="num">The course number</param>
        /// <param name="season">The season part of the semester for the class the assignment belongs to</param>
        /// <param name="year">The year part of the semester for the class the assignment belongs to</param>
        /// <param name="category">The name of the assignment category in the class</param>
        /// <param name="asgname">The new assignment name</param>
        /// <param name="asgpoints">The max point value for the new assignment</param>
        /// <param name="asgdue">The due DateTime for the new assignment</param>
        /// <param name="asgcontents">The contents of the new assignment</param>
        /// <returns>A JSON object containing success = true/false</returns>
        public IActionResult CreateAssignment(string subject, int num, string season, int year, string category, string asgname, int asgpoints, DateTime asgdue, string asgcontents)
        {
            // Error Handling 
            if ( string.IsNullOrEmpty(asgname)  || asgpoints == 0 || string.IsNullOrEmpty(asgcontents)) {
                return Json(new { success = false, message = "Null or empty parameter passed" });
            }

            // Make sure due date is passed current time 
            if ( asgdue < DateTime.Now) {
                return Json(new { success = false, message = "Due date must be passed current time" });
            }

            var existingAssignment = (from co in db.Courses 
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
            // Check if successful 
            if ( existingAssignment != null ) {
                return Json(new { success = false, message = "Assignment already exists"});
            }

            // Extract classID
            var classId = (from co in db.Courses 
                           join c in db.Classes on co.Id equals c.Course
                           where co.Dept == subject
                            && co.Number == num 
                            && c.Semester == season 
                            && c.Year == year
                           select c.Id).FirstOrDefault();
            // Check if successful 
            if ( classId <= 0 ) {
                return Json(new { success = false, message = "ClassID not found"});
            }

            // Extract categoryID
            var categoryID = (from cat in db.AssignmentCategories 
                              where cat.Name == category
                                && cat.Class == classId
                              select cat.Id).FirstOrDefault();

            // Check if successful 
            if ( categoryID <= 0 ) {
                return Json(new { success = false, message = "CategoryID not found"});
            }

            // Create assignment, add to database, and update grades
             Assignment newAssignment = new Assignment {
                Name = asgname,
                Due = asgdue,
                Points = (uint)asgpoints,
                Instructions = asgcontents,
                Category = categoryID
            };

            // Extract studentIDs
            var studentIds = (from e in db.Enrollments
                      join s in db.Students on e.Student equals s.Id
                      join c in db.Classes on e.Class equals c.Id
                      where e.Class == classId
                      select s.Id).ToList();

            foreach (var id in studentIds) {
                var enrollment = (from e in db.Enrollments
                                join s in db.Students on e.Student equals s.Id
                                where e.Student == id
                                    && e.Class == classId
                                select e).FirstOrDefault();

                if (enrollment != null && enrollment.Grade != "--") {
                    enrollment.Grade = CalculateGrade(id, classId);
                }
            }
            // Add and save the new assignment
            db.Assignments.Add(newAssignment);
            db.SaveChanges();
            return Json(new { success = true});
        }


        /// <summary>
        /// Gets a JSON array of all the submissions to a certain assignment.
        /// Each object in the array should have the following fields:
        /// "fname" - first name
        /// "lname" - last name
        /// "uid" - user ID
        /// "time" - DateTime of the submission
        /// "score" - The score given to the submission
        /// 
        /// </summary>
        /// <param name="subject">The course subject abbreviation</param>
        /// <param name="num">The course number</param>
        /// <param name="season">The season part of the semester for the class the assignment belongs to</param>
        /// <param name="year">The year part of the semester for the class the assignment belongs to</param>
        /// <param name="category">The name of the assignment category in the class</param>
        /// <param name="asgname">The name of the assignment</param>
        /// <returns>The JSON array</returns>
        public IActionResult GetSubmissionsToAssignment(string subject, int num, string season, int year, string category, string asgname)
        {
            var classID = (from co in db.Courses join 
                          c in db.Classes on co.Id equals c.Course
                          where co.Dept == subject
                            && co.Number == num 
                            && c.Semester == season 
                            && c.Year == year
                         select c.Id).FirstOrDefault();
            // Check if successful 
            if ( classID <= 0  ) {
                return Json(new { success = false, message = "ClassID not found"});
            }

            var categoryID = (from cat in db.AssignmentCategories 
                             where cat.Name == category
                                && cat.Class == classID
                             select cat.Id).FirstOrDefault();

            // Check if successful 
            if ( categoryID <= 0 ) {
                return Json(new { success = false, message = "CategoryID not found"});
            }

            var submissionQuery = from sub in db.Submissions
                                  join a in db.Assignments on sub.Assignment equals a.Id
                                  join s in db.Students on sub.Student equals s.Id
                                  where a.Category == categoryID
                                    && a.Name == asgname
                                  select new 
                                    {
                                        fname = s.First,
                                        lname = s.Last,
                                        uid = s.Id, 
                                        time = sub.Time,
                                        score = sub.Score
                                    };

            return Json(submissionQuery.ToArray());
        }

        /// <summary>
        /// Set the score of an assignment submission
        /// A professor scores a student's submission. The student's grade for the class should be updated. 
        /// Modify your GradeSubmission method to have this side-effect.
        /// </summary>
        /// <param name="subject">The course subject abbreviation</param>
        /// <param name="num">The course number</param>
        /// <param name="season">The season part of the semester for the class the assignment belongs to</param>
        /// <param name="year">The year part of the semester for the class the assignment belongs to</param>
        /// <param name="category">The name of the assignment category in the class</param>
        /// <param name="asgname">The name of the assignment</param>
        /// <param name="uid">The uid of the student who's submission is being graded</param>
        /// <param name="score">The new score for the submission</param>
        /// <returns>A JSON object containing success = true/false</returns>
        public IActionResult GradeSubmission(string subject, int num, string season, int year, string category, string asgname, string uid, int score)
        {
            var classID = (from co in db.Courses join 
                          c in db.Classes on co.Id equals c.Course
                          where co.Dept == subject
                            && co.Number == num 
                            && c.Semester == season 
                            && c.Year == year
                         select c.Id).FirstOrDefault();
            // Check if successful 
            if ( classID <= 0  ) {
                return Json(new { success = false, message = "ClassID not found"});
            }

            var submission = (from sub in db.Submissions
                              join a in db.Assignments on sub.Assignment equals a.Id
                              join cat in db.AssignmentCategories on a.Category equals cat.Id
                              join c in db.Classes on cat.Class equals c.Id
                              join s in db.Students on sub.Student equals s.Id
                              where c.Id == classID
                                && cat.Name == category
                                && a.Name == asgname
                                && s.Id == uid
                              select sub).FirstOrDefault();
            if (submission == null) {
                return Json(new { success = false, message = "Submission doesn't exist" });
            }
            
            submission.Score = (uint)score; 
            db.SaveChanges(); // To have updated score in table

            // Update student's grade
            var enrollment = (from e in db.Enrollments
                                where e.Student == uid
                                && e.Class == classID
                                select e).FirstOrDefault();

            if ( enrollment != null ) {
                enrollment.Grade = CalculateGrade(uid, classID);  
                db.SaveChanges();
                return Json(new { success = true});                  
            }
            return Json(new { success = false, message = "Unable to update grade" });
        }


        /// <summary>
        /// Returns a JSON array of the classes taught by the specified professor
        /// Each object in the array should have the following fields:
        /// "subject" - The subject abbreviation of the class (such as "CS")
        /// "number" - The course number (such as 5530)
        /// "name" - The course name
        /// "season" - The season part of the semester in which the class is taught
        /// "year" - The year part of the semester in which the class is taught
        /// </summary>
        /// <param name="uid">The professor's uid</param>
        /// <returns>The JSON array</returns>
        public IActionResult GetMyClasses(string uid)
        {
            var myClasses = from co in db.Courses 
                            join c in db.Classes on co.Id equals c.Course
                            join p in db.Professors on c.Professor equals p.Id 
                            where p.Id == uid 
                            select new 
                                {
                                    subject = co.Dept,
                                    number = co.Number, 
                                    name = co.Name, 
                                    season = c.Semester, 
                                    year = c.Year
                                }; 
            if ( myClasses == null ) {
                return Json(new { success = false, message = "No classes for passed through uid" });
            }         
            return Json(myClasses.ToArray());
        }

        // Letter grades should be calculated as follows:
        // 1. If a student does not have a submission for an assignment, the score for that assignment is treated as 0.
        // 2. If an AssignmentCategory does not have any assignments, it is not included in the calculation.
        // 3. For each non-empty category in the class:
            // Calculate the percentage of (total points earned / total max points) of all assignments in the category. This should be a number between 0.0 - 1.0. For example, if there are 875 possible points spread among various assignments in the category, and the student earned a total of 800 among all assignments in that category, this value should be ~0.914
            // Multiply the percentage by the category weight. For example, if the category weight is 15, then the scaled total for the category is ~13.71 (using the previous example).
        // 4. Compute the total of all scaled category totals from the previous step. IMPORTANT - there is no rule that assignment category weights must sum to 100. Therefore, we have to re-scale in the next step.
        // 5. Compute the scaling factor to make all category weights add up to 100%. This scaling factor is 100 / (sum of all category weights).
        // 6. Multiply the total score you computed in step 4 by the scaling factor you computed in step 5. This is the total percentage the student earned in the class.
        // 7. Convert the class percentage to a letter grade using the scale found in our class syllabus.
        public string CalculateGrade( string uid, uint classID  ) {
            
            // Extract needed information 
            var query = from c in db.Classes 
                        join cat in db.AssignmentCategories on c.Id equals cat.Class
                        join a in db.Assignments on cat.Id equals a.Category
                        join sub in db.Submissions on a.Id equals sub.Assignment
                        join s in db.Students on sub.Student equals s.Id
                        where s.Id == uid
                        where c.Id == classID
                        select new {
                            CategoryName = cat.Name, 
                            CategoryWeight = cat.Weight,
                            AssignmentName = a.Name,
                            PointsEarned = sub == null ? 0 : sub.Score,
                            PointsPossible = a.Points,
                        };
            var sumOfAllCategoryWeights = 0.0;
            foreach (var v in query) {
                System.Diagnostics.Debug.WriteLine("********** Query in CalcGrades *************");
                System.Diagnostics.Debug.WriteLine(v);
                sumOfAllCategoryWeights += v.CategoryWeight;
            }
            
            List<double> scaledCategoryTotals = new List<double>();
            foreach ( var q in query ) {
                if ( q.AssignmentName != null ) {
                    double scaledCategoryTotal = q.PointsEarned / (double) q.PointsPossible * q.CategoryWeight; 
                    scaledCategoryTotals.Add(scaledCategoryTotal);
                }
            }
            var sumOfCategoryTotals = scaledCategoryTotals.Sum();
            var scaledFactor = 100 / sumOfAllCategoryWeights;
            var percentEarnedInClass = sumOfCategoryTotals * scaledFactor;

            // Convert Class percentage to a letter grade using scale below 
            //  The letter grading policy is as follows:
            //  87-89  B+	77-79 C+	67-69 D+	 
            //  93-100 A	83-86 B	    73-76 C	    63-66 D	  0-59 E
            //  90-92  A-	80-82 B-	70-72 C-	60-62 D-
            string letterGrade;
            if (percentEarnedInClass >= 93) {
                letterGrade = "A";
            }
            else if (percentEarnedInClass >= 90) {
                letterGrade = "A-";
            }
            else if (percentEarnedInClass >= 87) {
                letterGrade = "B+";
            }
            else if (percentEarnedInClass >= 83) {
                letterGrade = "B";
            }
            else if (percentEarnedInClass >= 80) {
                letterGrade = "B-";
            }
            else if (percentEarnedInClass >= 77) {
                letterGrade = "C+";
            }
            else if (percentEarnedInClass >= 73) {
                letterGrade = "C";
            }
            else if (percentEarnedInClass >= 70) {
                letterGrade = "C-";
            }
            else if (percentEarnedInClass >= 67) {
                letterGrade = "D+";
            }
            else if (percentEarnedInClass >= 63) {
                letterGrade = "D";
            }
            else if (percentEarnedInClass >= 60) {
                letterGrade = "D-";
            }
            else {
                letterGrade = "E";
            }

            return letterGrade;
        }
        /*******End code to modify********/
    }
}

