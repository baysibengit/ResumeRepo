using System;
using System.Collections.Generic;

namespace LMS.Models.LMSModels;

public partial class Administrator
{
    public string Id { get; set; } = null!;

    public string First { get; set; } = null!;

    public string Last { get; set; } = null!;

    public DateOnly Dob { get; set; }
}
