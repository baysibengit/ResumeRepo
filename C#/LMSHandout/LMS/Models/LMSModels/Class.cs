using System;
using System.Collections.Generic;

namespace LMS.Models.LMSModels;

public partial class Class
{
    public uint Id { get; set; }

    public uint Year { get; set; }

    public string Semester { get; set; } = null!;

    public string Location { get; set; } = null!;

    public TimeOnly StartTime { get; set; }

    public TimeOnly EndTime { get; set; }

    public uint Course { get; set; }

    public string Professor { get; set; } = null!;

    public virtual ICollection<AssignmentCategory> AssignmentCategories { get; set; } = new List<AssignmentCategory>();

    public virtual Course CourseNavigation { get; set; } = null!;

    public virtual ICollection<Enrollment> Enrollments { get; set; } = new List<Enrollment>();

    public virtual Professor ProfessorNavigation { get; set; } = null!;
}
