using System;
using System.Collections.Generic;

namespace LMS.Models.LMSModels;

public partial class Enrollment
{
    public string Grade { get; set; } = null!;

    public string Student { get; set; } = null!;

    public uint Class { get; set; }

    public virtual Class ClassNavigation { get; set; } = null!;

    public virtual Student StudentNavigation { get; set; } = null!;
}
